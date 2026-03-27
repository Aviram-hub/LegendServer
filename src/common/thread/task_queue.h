/**
 * @file task_queue.h
 * @brief 任务队列
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "common/thread/mutex.h"
#include "condition.h"
#include <queue>

namespace legend {

/**
 * @brief 任务类型
 */
using Task = std::function<void()>;

/**
 * @brief 线程安全任务队列
 */
class TaskQueue : public NonCopyable {
public:
    TaskQueue() = default;
    ~TaskQueue() = default;

    // 添加任务
    void push(Task task) {
        MutexGuard guard(mutex_);
        tasks_.push(std::move(task));
        notEmpty_.notify();
    }

    // 尝试添加任务（非阻塞）
    void tryPush(Task task) {
        MutexGuard guard(mutex_);
        tasks_.push(std::move(task));
        notEmpty_.notify();
    }

    // 获取任务（阻塞）
    Task pop() {
        MutexGuard guard(mutex_);
        while (tasks_.empty() && !stopped_) {
            notEmpty_.wait(mutex_);
        }

        if (stopped_) {
            return nullptr;
        }

        Task task = std::move(tasks_.front());
        tasks_.pop();
        return task;
    }

    // 尝试获取任务（非阻塞）
    bool tryPop(Task& task) {
        MutexGuard guard(mutex_);
        if (tasks_.empty()) {
            return false;
        }
        task = std::move(tasks_.front());
        tasks_.pop();
        return true;
    }

    // 获取任务（带超时）
    bool popFor(Task& task, uint32 milliseconds) {
        MutexGuard guard(mutex_);
        while (tasks_.empty() && !stopped_) {
            if (!notEmpty_.waitFor(mutex_, milliseconds)) {
                return false;
            }
        }

        if (stopped_ || tasks_.empty()) {
            return false;
        }

        task = std::move(tasks_.front());
        tasks_.pop();
        return true;
    }

    // 获取所有任务
    void popAll(std::queue<Task>& tasks) {
        MutexGuard guard(mutex_);
        tasks = std::move(tasks_);
    }

    // 队列是否为空
    bool empty() const {
        MutexGuard guard(mutex_);
        return tasks_.empty();
    }

    // 获取队列大小
    size_t size() const {
        MutexGuard guard(mutex_);
        return tasks_.size();
    }

    // 停止队列
    void stop() {
        MutexGuard guard(mutex_);
        stopped_ = true;
        notEmpty_.notifyAll();
    }

    // 重置队列
    void reset() {
        MutexGuard guard(mutex_);
        stopped_ = false;
        while (!tasks_.empty()) {
            tasks_.pop();
        }
    }

    // 是否已停止
    bool stopped() const {
        MutexGuard guard(mutex_);
        return stopped_;
    }

private:
    mutable Mutex mutex_;
    Condition notEmpty_;
    std::queue<Task> tasks_;
    bool stopped_ = false;
};

/**
 * @brief 优先级任务队列
 */
class PriorityTaskQueue : public NonCopyable {
public:
    struct PriorityTask {
        int priority;
        uint64 sequence;
        Task task;

        bool operator<(const PriorityTask& other) const {
            if (priority != other.priority) {
                return priority < other.priority;
            }
            return sequence > other.sequence;
        }
    };

    void push(Task task, int priority = 0) {
        MutexGuard guard(mutex_);
        tasks_.push({priority, nextSequence_++, std::move(task)});
        notEmpty_.notify();
    }

    Task pop() {
        MutexGuard guard(mutex_);
        while (tasks_.empty() && !stopped_) {
            notEmpty_.wait(mutex_);
        }

        if (stopped_) {
            return nullptr;
        }

        Task task = std::move(tasks_.top().task);
        tasks_.pop();
        return task;
    }

    bool empty() const {
        MutexGuard guard(mutex_);
        return tasks_.empty();
    }

    size_t size() const {
        MutexGuard guard(mutex_);
        return tasks_.size();
    }

    void stop() {
        MutexGuard guard(mutex_);
        stopped_ = true;
        notEmpty_.notifyAll();
    }

private:
    mutable Mutex mutex_;
    Condition notEmpty_;
    std::priority_queue<PriorityTask> tasks_;
    uint64 nextSequence_ = 0;
    bool stopped_ = false;
};

} // namespace legend