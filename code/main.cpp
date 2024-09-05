#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <stdexcept>
#include <chrono>
#include <exception>

// Task structure
struct Task {
    std::function<void()> function;
};

// Task queue class
class TaskQueue {
public:
    explicit TaskQueue(size_t size) : capacity(size) {}

    void push(Task task) {
        std::unique_lock<std::mutex> lock(mutex);
        // Wait until there is space in the queue
        condition_full.wait(lock, [this]() { return queue.size() < capacity || shutdown; });
        if (shutdown) {
            throw std::runtime_error("Cannot add tasks to a shutdown queue");
        }
        queue.push(task);
        condition_empty.notify_one();
    }

    Task pop() {
        std::unique_lock<std::mutex> lock(mutex);
        // Wait until there is at least one task in the queue
        condition_empty.wait(lock, [this]() { return !queue.empty() || shutdown; });
        if (shutdown && queue.empty()) {
            throw std::runtime_error("Cannot pop tasks from a shutdown queue");
        }
        Task task = queue.front();
        queue.pop();
        condition_full.notify_one();
        return task;
    }

    bool is_empty() const {
        std::unique_lock<std::mutex> lock(mutex);
        return queue.empty();
    }

    void set_shutdown() {
        std::unique_lock<std::mutex> lock(mutex);
        shutdown = true;
        condition_empty.notify_all();
        condition_full.notify_all();
    }

private:
    std::queue<Task> queue;
    mutable std::mutex mutex;
    std::condition_variable condition_full;
    std::condition_variable condition_empty;
    size_t capacity;
    bool shutdown = false;
};

// Thread pool class
class ThreadPool {
public:
    ThreadPool(size_t thread_count, size_t queue_size)
        : task_queue(queue_size), shutdown(false) {
        for (size_t i = 0; i < thread_count; ++i) {
            threads.emplace_back(&ThreadPool::worker, this);
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mutex);
            shutdown = true;
        }
        task_queue.set_shutdown();
        condition.notify_all();
        for (std::thread &thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void add_task(std::function<void()> function) {
        {
            std::unique_lock<std::mutex> lock(mutex);
            if (shutdown) {
                throw std::runtime_error("Cannot add task to a shutdown thread pool");
            }
        }
        task_queue.push({function});
        condition.notify_one();
    }

private:
    void worker() {
        while (true) {
            Task task;
            try {
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    condition.wait(lock, [this]() { return shutdown || !task_queue.is_empty(); });
                    if (shutdown && task_queue.is_empty()) {
                        return;
                    }
                    task = task_queue.pop();
                }
                task.function();
            } catch (const std::exception& e) {
                std::cerr << "Task failed: " << e.what() << std::endl;
            }
        }
    }
    TaskQueue task_queue;
    std::vector<std::thread> threads;
    std::mutex mutex;
    std::condition_variable condition;
    bool shutdown;
};
// Example task function
void example_task(int num) {
    std::cout << "Task " << num << " is being processed.\n";
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate task processing
}
int main() {
    try {
        ThreadPool pool(4, 10); // Create a thread pool with 4 threads and a queue size of 10

        for (int i = 0; i < 10; ++i) {
            pool.add_task([i]() { example_task(i + 1); });
        }

        std::this_thread::sleep_for(std::chrono::seconds(5)); // Wait for tasks to complete
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
