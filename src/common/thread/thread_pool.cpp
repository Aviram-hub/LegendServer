/**
 * @file thread_pool.cpp
 * @brief 线程池实现
 */

#include "thread_pool.h"
#include <algorithm>

namespace legend {

ThreadPool::ThreadPool(const String& name)
    : name_(name)
    , started_(false)
    , activeWorkers_(0)
    , idleWorkers_(0) {
}

ThreadPool::ThreadPool(const ThreadPoolConfig& config, const String& name)
    : name_(name)
    , config_(config)
    , started_(false)
    , activeWorkers_(0)
    , idleWorkers_(0) {
}

ThreadPool::~ThreadPool() {
    if (started_) {
        stop();
    }
}

void ThreadPool::start(uint32 numThreads) {
    if (started_) {
        return;
    }

    started_ = true;

    if (numThreads == 0) {
        numThreads = config_.minThreads;
    }

    numThreads = std::min(numThreads, config_.maxThreads);

    workers_.reserve(numThreads);
    for (uint32 i = 0; i < numThreads; ++i) {
        workers_.emplace_back(new Thread(
            [this]() { workerThread(); },
            name_ + "-" + std::to_string(i)
        ));
        workers_.back()->start();
    }
}

void ThreadPool::stop() {
    if (!started_) {
        return;
    }

    started_ = false;
    taskQueue_.stop();

    for (auto& worker : workers_) {
        if (worker) {
            worker->join();
        }
    }

    workers_.clear();
}

void ThreadPool::wait() {
    while (taskQueue_.size() > 0 || activeWorkers_ > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void ThreadPool::execute(Task task) {
    if (started_) {
        taskQueue_.push(std::move(task));
    }
}

uint32 ThreadPool::workerCount() const {
    MutexGuard guard(mutex_);
    return static_cast<uint32>(workers_.size());
}

uint32 ThreadPool::idleCount() const {
    return idleWorkers_.load();
}

size_t ThreadPool::queueSize() const {
    return taskQueue_.size();
}

void ThreadPool::workerThread() {
    while (started_) {
        Task task = taskQueue_.pop();
        if (task) {
            ++activeWorkers_;
            --idleWorkers_;

            try {
                task();
            } catch (...) {
                // 异常处理
            }

            --activeWorkers_;
            ++idleWorkers_;
        }
    }
}

void ThreadPool::createWorker() {
    MutexGuard guard(mutex_);
    if (workers_.size() < config_.maxThreads) {
        workers_.emplace_back(new Thread(
            [this]() { workerThread(); },
            name_ + "-" + std::to_string(workers_.size())
        ));
        workers_.back()->start();
    }
}

void ThreadPool::destroyIdleWorkers() {
    // 销毁空闲线程的逻辑
    // 实际实现中需要更复杂的逻辑
}

// FixedThreadPool 实现
FixedThreadPool::FixedThreadPool(uint32 numThreads, const String& name)
    : name_(name)
    , numThreads_(numThreads)
    , started_(false) {
}

FixedThreadPool::~FixedThreadPool() {
    if (started_) {
        stop();
    }
}

void FixedThreadPool::start() {
    if (started_) {
        return;
    }

    started_ = true;
    workers_.reserve(numThreads_);

    for (uint32 i = 0; i < numThreads_; ++i) {
        workers_.emplace_back(new Thread(
            [this]() { workerThread(); },
            name_ + "-" + std::to_string(i)
        ));
        workers_.back()->start();
    }
}

void FixedThreadPool::stop() {
    if (!started_) {
        return;
    }

    started_ = false;
    taskQueue_.stop();

    for (auto& worker : workers_) {
        if (worker) {
            worker->join();
        }
    }

    workers_.clear();
}

void FixedThreadPool::execute(Task task) {
    if (started_) {
        taskQueue_.push(std::move(task));
    }
}

void FixedThreadPool::workerThread() {
    while (started_) {
        Task task = taskQueue_.pop();
        if (task) {
            try {
                task();
            } catch (...) {
                // 异常处理
            }
        }
    }
}

} // namespace legend