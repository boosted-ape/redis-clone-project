#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
public:
    // Constructor that initializes the thread pool with a specified number of threads
    explicit ThreadPool(size_t num_threads = std::thread::hardware_concurrency());

    // Destructor that joins all threads
    ~ThreadPool();

    // Method to enqueue a task for execution
    void enqueue(std::function<void()> task);

private:
    // Vector to hold the worker threads
    std::vector<std::thread> threads_;
    
    // Queue to hold the tasks
    std::queue<std::function<void()>> tasks_;
    
    // Mutex to protect access to the task queue
    std::mutex queue_mutex_;
    
    // Condition variable for thread synchronization
    std::condition_variable cv_;
    
    // Flag to indicate if the thread pool is stopping
    bool stop_ = false;
};

#endif // THREADPOOL_H
