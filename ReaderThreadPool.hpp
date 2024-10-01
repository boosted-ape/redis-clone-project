#ifndef READER_THREAD_POOL_HPP
#define READER_THREAD_POOL_HPP

#include <cstddef> // for size_t
#include <thread>  // for std::thread
#include "ThreadPool.hpp" // Ensure ThreadPool is defined properly

class ReaderThreadPool : public ThreadPool {
public:
    // Constructor with a default number of threads
    explicit ReaderThreadPool(size_t num_threads = std::thread::hardware_concurrency() / 2);
};

#endif // READER_THREAD_POOL_HPP
