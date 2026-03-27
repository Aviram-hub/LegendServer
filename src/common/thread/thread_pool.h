/**
 * @file thread_pool.h
 * @brief 线程池
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "thread.h"
#include "task_queue.h"
#include <vector>
#include <future>

namespace legend {

/**
 * @brief 线程池配置
 */
struct ThreadPoolConfig {
    uint32 minThreads = 2;      // 最小线程数
    uint32 maxThreads = 8;      // 最大线程数
    uint32 maxQueueSize = 1000; // 最大队列大小
    uint32 idleTimeout = 60000; // 空闲超时(ms)
};

/**
 * @brief 线程池类
 */
class ThreadPool : public NonCopyable {
public:
    explicit ThreadPool(const String& name = "ThreadPool");
    explicit ThreadPool(const ThreadPoolConfig& config, const String& name = "ThreadPool");
    ~ThreadPool();

    // 启动线程池
    void start(uint32 numThreads = 0);

    // 停止线程池
    void stop();

    // 等待所有任务完成
    void wait();

    // 提交任务
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type>;

    // 添加任务（无返回值）
    void execute(Task task);

    // 获取线程池名称
    const String& name() const { return name_; }

    // 获取工作线程数
    uint32 workerCount() const;

    // 获取空闲线程数
    uint32 idleCount() const;

    // 获取队列大小
    size_t queueSize() const;

    // 线程池是否已启动
    bool started() const { return started_; }

private:
    void workerThread();
    void createWorker();
    void destroyIdleWorkers();

    String name_;
    ThreadPoolConfig config_;
    std::vector<UniquePtr<Thread>> workers_;
    TaskQueue taskQueue_;

    mutable Mutex mutex_;
    Condition condition_;

    std::atomic<bool> started_;
    std::atomic<uint32> activeWorkers_;
    std::atomic<uint32> idleWorkers_;
};

// 模板方法实现
template<typename F, typename... Args>
auto ThreadPool::submit(F&& f, Args&&... args)
    -> std::future<typename std::invoke_result<F, Args...>::type> {
    using ReturnType = typename std::invoke_result<F, Args...>::type;

    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<ReturnType> result = task->get_future();

    taskQueue_.push([task]() {
        (*task)();
    });

    return result;
}

/**
 * @brief 固定大小线程池
 */
class FixedThreadPool : public NonCopyable {
public:
    explicit FixedThreadPool(uint32 numThreads, const String& name = "FixedThreadPool");
    ~FixedThreadPool();

    void start();
    void stop();

    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type>;

    void execute(Task task);

    const String& name() const { return name_; }
    size_t queueSize() const { return taskQueue_.size(); }
    bool started() const { return started_; }

private:
    void workerThread();

    String name_;
    uint32 numThreads_;
    std::vector<UniquePtr<Thread>> workers_;
    TaskQueue taskQueue_;
    std::atomic<bool> started_;
};

template<typename F, typename... Args>
auto FixedThreadPool::submit(F&& f, Args&&... args)
    -> std::future<typename std::invoke_result<F, Args...>::type> {
    using ReturnType = typename std::invoke_result<F, Args...>::type;

    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<ReturnType> result = task->get_future();

    taskQueue_.push([task]() {
        (*task)();
    });

    return result;
}

} // namespace legend