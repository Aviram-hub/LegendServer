/**
 * @file session_pool.h
 * @brief 会话池
 */

#pragma once

#include "types.h"
#include "noncopyable.h"
#include "session.h"
#include "common/thread/mutex.h"
#include <vector>

namespace legend {

/**
 * @brief 会话池
 * 预分配会话对象，减少内存分配开销
 */
class SessionPool : public NonCopyable {
public:
    explicit SessionPool(size_t initialSize = 1024);
    ~SessionPool() = default;

    // 获取会话
    Ptr<Session> acquire();

    // 释放会话
    void release(Ptr<Session> session);

    // 获取当前池大小
    size_t size() const {
        MutexGuard guard(mutex_);
        return pool_.size();
    }

    // 获取总容量
    size_t capacity() const {
        return capacity_;
    }

    // 预分配
    void preallocate(size_t count);

private:
    mutable Mutex mutex_;
    std::vector<Ptr<Session>> pool_;
    size_t capacity_;
    std::atomic<int64> nextId_{0};
};

} // namespace legend