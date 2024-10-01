#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

using namespace std;

class ThreadPool
{
public:
    ThreadPool(size_t num_threads = thread::hardware_concurrency())
    {
        for (size_t i = 0; i < num_threads; ++i)
        {
            threads_.emplace_back([this]
                                  {
                while (true) {
                    function<void()> task;
                    // The reason for putting the below code
                    // here is to unlock the queue before
                    // executing the task so that other
                    // threads can perform enqueue tasks
                    {

                        unique_lock<mutex> lock(
                            queue_mutex_);

                        cv_.wait(lock, [this] {
                            return !tasks_.empty() || stop_;
                        });

                        // exit the thread in case the pool
                        // is stopped and there are no tasks
                        if (stop_ && tasks_.empty()) {
                            return;
                        }

                        // Get the next task from the queue
                        task = move(tasks_.front());
                        tasks_.pop();
                    }

                    task();
                } });
        }
    }
    ~ThreadPool()
    {
        {
            // Lock the queue to update the stop flag safely
            unique_lock<mutex> lock(queue_mutex_);
            stop_ = true;
        }

        // Notify all threads
        cv_.notify_all();

        // Joining all worker threads to ensure they have
        // completed their tasks
        for (auto &thread : threads_)
        {
            thread.join();
        }
    }
    void enqueue(function<void()> task)
    {
        {
            unique_lock<std::mutex> lock(queue_mutex_);
            tasks_.emplace(move(task));
        }
        cv_.notify_one();
    }

private:
    vector<thread> threads_;
    queue<function<void()>> tasks_;
    mutex queue_mutex_;
    condition_variable cv_;
    bool stop_ = false;
};