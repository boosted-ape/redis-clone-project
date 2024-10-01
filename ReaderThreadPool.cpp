#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include "ThreadPool.hpp" // Use quotes for local headers

using namespace std;

// Correct the inheritance syntax
class ReaderThreadPool : public ThreadPool {
public:
    // Constructor with a default number of threads
    ReaderThreadPool(size_t num_threads = thread::hardware_concurrency() / 2)
        : ThreadPool(num_threads) {}
};
