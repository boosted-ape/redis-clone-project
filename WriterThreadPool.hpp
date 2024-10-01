#ifndef WRITER_THREAD_POOL_HPP
#define WRITER_THREAD_POOL_HPP

#include <cstddef> // for size_t
#include <thread>  // for std::thread
#include "ThreadPool.hpp" // Ensure ThreadPool is defined properly

class WriterThreadPool : public ThreadPool {
public:
    // Constructor with a default number of threads
    explicit WriterThreadPool(size_t num_threads = std::thread::hardware_concurrency() / 2);
};

#endif // WRITER_THREAD_POOL_HPP
