/**
 * @file condition.h
 * @brief 条件变量封装
 */

#pragma once

#include "types.h"
#include "noncopyable.h"
#include "mutex.h"
#include <pthread.h>
#include <ctime>

namespace legend {

/**
 * @brief 条件变量类
 */
class Condition : public NonCopyable {
public:
    Condition() {
        pthread_cond_init(&cond_, nullptr);
    }

    ~Condition() {
        pthread_cond_destroy(&cond_);
    }

    // 等待条件
    void wait(Mutex& mutex) {
        pthread_cond_wait(&cond_, mutex.get());
    }

    // 等待条件（带超时，毫秒）
    bool waitFor(Mutex& mutex, uint32 milliseconds) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += milliseconds / 1000;
        ts.tv_nsec += (milliseconds % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }
        return pthread_cond_timedwait(&cond_, mutex.get(), &ts) == 0;
    }

    // 唤醒一个等待线程
    void notify() {
        pthread_cond_signal(&cond_);
    }

    // 唤醒所有等待线程
    void notifyAll() {
        pthread_cond_broadcast(&cond_);
    }

private:
    pthread_cond_t cond_;
};

/**
 * @brief 信号量（基于条件变量）
 */
class Semaphore : public NonCopyable {
public:
    explicit Semaphore(uint32 count = 0)
        : count_(count) {
    }

    // 获取信号量
    void wait() {
        MutexGuard guard(mutex_);
        while (count_ == 0) {
            cond_.wait(mutex_);
        }
        --count_;
    }

    // 尝试获取信号量
    bool tryWait() {
        MutexGuard guard(mutex_);
        if (count_ > 0) {
            --count_;
            return true;
        }
        return false;
    }

    // 尝试获取信号量（带超时）
    bool waitFor(uint32 milliseconds) {
        MutexGuard guard(mutex_);
        while (count_ == 0) {
            if (!cond_.waitFor(mutex_, milliseconds)) {
                return false;
            }
        }
        --count_;
        return true;
    }

    // 释放信号量
    void post() {
        MutexGuard guard(mutex_);
        ++count_;
        cond_.notify();
    }

    // 获取当前计数
    uint32 getCount() const {
        MutexGuard guard(mutex_);
        return count_;
    }

private:
    mutable Mutex mutex_;
    Condition cond_;
    uint32 count_;
};

/**
 * @brief 倒计时门闩
 */
class CountDownLatch : public NonCopyable {
public:
    explicit CountDownLatch(uint32 count)
        : count_(count) {
    }

    void wait() {
        MutexGuard guard(mutex_);
        while (count_ > 0) {
            cond_.wait(mutex_);
        }
    }

    bool waitFor(uint32 milliseconds) {
        MutexGuard guard(mutex_);
        while (count_ > 0) {
            if (!cond_.waitFor(mutex_, milliseconds)) {
                return false;
            }
        }
        return true;
    }

    void countDown() {
        MutexGuard guard(mutex_);
        if (count_ > 0) {
            --count_;
            if (count_ == 0) {
                cond_.notifyAll();
            }
        }
    }

    uint32 getCount() const {
        MutexGuard guard(mutex_);
        return count_;
    }

private:
    mutable Mutex mutex_;
    Condition cond_;
    uint32 count_;
};

} // namespace legend