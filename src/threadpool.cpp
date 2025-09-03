#include "threadpool.h"

namespace cppagent
{
ThreadPool::ThreadPool(size_t capacity)
    : stop_(false)
{
    for(size_t i = 0; i < capacity; ++i) {
        workers_.emplace_back([this] {
            for(;;) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    condition_.wait(lock, [this] {
                        return stop_ || !tasks_.empty();
                    });
                    if(stop_ && tasks_.empty()) {
                        return;
                    }
                    task = std::move(tasks_.front());
                    tasks_.pop();
                }
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        stop_ = true;
    }
    condition_.notify_all();
    for(std::thread& worker: workers_) {
        worker.join();
    }
}
} // namespace cppagent
