Bug Report
Title: Thread Pool Implementation
________________________________________
Description
The thread pool implementation in C++ has several issues, including improper handling of task queue overflow, potential race conditions, and inadequate error handling. These issues could lead to task mismanagement, program crashes, or unpredictable behaviour under concurrent load.
________________________________________
Steps to Reproduce
1.	Compile the Code:
o	Save the provided code as main.cpp.
o	Compile the code using: g++ -o thread_pool main.cpp -pthread.
2.	Run the Executable:
o	Execute the program with: ./thread_pool.
3.	Test Queue Overflow Handling:
o	Adjust the queue size and add a number of tasks that exceed the queue capacity to observe behavior.
4.	Stress Test Concurrent Task Addition:
o	Modify the code to add tasks from multiple threads simultaneously to test thread safety and synchronization.
________________________________________
Expected Behaviour
1.	Queue Overflow Handling:
o	The system should handle task overflow gracefully, either by blocking or by rejecting new tasks when the queue is full.
2.	Thread Safety:
o	Access to the shared task queue should be properly synchronized to prevent race conditions.
3.	Error Handling:
o	Tasks should not be added if the thread pool is in a shutdown state, and appropriate exceptions should be thrown.
________________________________________
Actual Behaviour
1.	Queue Overflow Handling:
o	The queue might not handle overflow scenarios correctly, potentially causing crashes or undefined behaviour when the queue is full.
2.	Thread Safety Issues:
o	There could be race conditions due to improper synchronization, leading to unpredictable behaviour when tasks are added or removed concurrently.
3.	Error Handling:
o	The code may not correctly handle errors related to adding tasks after the thread pool has been shut down, leading to possible exceptions or program crashes.
________________________________________
Code Affected
1.	TaskQueue Class:
o	Methods: push and pop
o	Issue: Improper handling of queue capacity and synchronization.
2.	ThreadPool Class:
o	Methods: add_task and worker
o	Issue: Potential race conditions and improper handling of the shutdown state.
________________________________________
Suggested Fixes
1.	Update TaskQueue::push:
o	Ensure that the push method blocks or handles cases where the queue is full. Properly synchronize access to the queue to prevent race conditions.
void push(Task task) {
    std::unique_lock<std::mutex> lock(mutex);
    // Wait until there is space in the queue
    condition_full.wait(lock, [this]() { return queue.size() < capacity; });
    queue.push(task);
    condition_empty.notify_one();
}
2.	Update ThreadPool::add_task:
o	Modify the add_task method to handle errors when attempting to add tasks after shutdown has been initiated.
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
3.	Update ThreadPool::worker:
o	Improve the worker method to ensure correct handling of shutdown signals and proper processing of tasks.
void worker() {
    while (true) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(mutex);
            condition.wait(lock, [this]() { return shutdown || !task_queue.is_empty(); });
            if (shutdown && task_queue.is_empty()) {
                return;
            }
            task = task_queue.pop();
        }
        task.function();
    }
}
________________________________________
Testing After Fixes
1.	Recompile and Run:
o	Ensure the code compiles without errors.
o	Run the executable and observe task handling, especially during high load and concurrent task addition.
2.	Test Overflow Handling:
o	Validate that the queue correctly handles overflow situations by adding tasks beyond the queue’s capacity.
3.	Verify Thread Safety:
o	Stress test with multiple concurrent task additions to ensure proper synchronization and prevent race conditions.
4.	Check Error Handling:
o	Verify that attempting to add tasks after shutdown triggers appropriate exceptions and does not lead to undefined behaviour.

