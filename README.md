# Thread Pool Implementation

## Overview

This project provides a C++ implementation of a thread pool, a concurrency pattern designed to efficiently manage and execute multiple tasks. The thread pool maintains a fixed number of worker threads that process tasks from a shared queue, optimizing resource utilization and reducing the overhead associated with thread management.

## Objectives

The primary goal of this project is to implement a thread pool that supports concurrent task execution using a fixed number of threads. Key features include:
- Efficient task scheduling
- Thread-safe queue management
- Graceful shutdown of threads

## Components

### TaskQueue Class

The `TaskQueue` class manages a queue of tasks with thread-safe operations, ensuring that tasks can be added and removed without causing race conditions.

- **Constructor**: `TaskQueue(size_t size)`  
  Initializes the queue with a specified capacity.

- **void push(Task task)**  
  Adds a task to the queue. This operation blocks if the queue is full, ensuring tasks are added in a controlled manner.

- **Task pop()**  
  Removes and returns a task from the queue. This operation blocks if the queue is empty, ensuring tasks are only removed when available.

- **bool is_empty() const**  
  Checks if the queue is currently empty.

### ThreadPool Class

The `ThreadPool` class manages a pool of worker threads and a task queue. It coordinates the execution of tasks and manages the lifecycle of the threads.

- **Constructor**: `ThreadPool(size_t thread_count, size_t queue_size)`  
  Initializes the thread pool with a specified number of threads and a task queue of a given size.

- **Destructor**  
  Ensures that all threads are properly joined before the thread pool is destroyed, preventing resource leaks.

- **void add_task(std::function<void()> function)**  
  Adds a task to the queue. Worker threads execute tasks as they become available.

## Important Note

Ensure that your development environment supports C++11 or later. This implementation relies on C++11 features for threading and concurrency, and may not work correctly with older versions of the C++ standard.

## Contributing

We welcome contributions to enhance and improve the thread pool implementation. Here's how you can contribute:

- **Open Issues**: If you encounter bugs or have feature requests, please open an issue on our GitHub repository.
- **Submit Pull Requests**: Feel free to fork the repository and submit pull requests for bug fixes, improvements, or new features.
- **Discuss Major Changes**: For significant changes or enhancements, please discuss them with us before making modifications. This helps coordinate efforts and ensures that changes align with the project's goals.

## License

This project is licensed under the MIT License. Review the terms of the license in the [LICENSE](./LICENSE) file included in this repository.

### Explanation:

- **Compilation:** Includes the command for manually compiling the project.
- **Running the Executable:** Provides the command to run the compiled program.
- **Using Makefile:** Describes how to use the `Makefile` with the `make` command, including build, run, and clean targets.
- **Description and License:** You can add any additional information about your project, including a description and license information if needed.


## References

- **[C++ Standard Library Documentation](https://en.cppreference.com/)**: Official reference for C++ standard library functions and classes used in the project.
- **[Concurrency in C++](https://www.modernescpp.com/index.php/concurrency)**: A comprehensive resource for understanding and implementing concurrency features in C++.
- **[GNU Make Documentation](https://www.gnu.org/software/make/manual/make.html)**: A guide on writing and understanding Makefiles for automating the build process.
- **[Thread Pool Design Patterns](https://en.wikipedia.org/wiki/Thread_pool_pattern)**: Patterns and best practices for implementing thread pools in C++.

## Compile and Run

To compile the code, use the following commands:

```bash
g++ -std=c++11 -pthread -o threadpool main.cpp
./threadpool

or

make
make run
make clean
