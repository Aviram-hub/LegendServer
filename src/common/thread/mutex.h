/**
 * @file mutex.h
 * @brief 互斥锁封装
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include <pthread.h>
#include <cassert>

namespace legend {

/**
 * @brief 互斥锁类
 */
class Mutex : public NonCopyable {
public:
    Mutex() {
        pthread_mutex_init(&mutex_, nullptr);
    }

    ~Mutex() {
        pthread_mutex_destroy(&mutex_);
    }

    void lock() {
        pthread_mutex_lock(&mutex_);
    }

    void unlock() {
        pthread_mutex_unlock(&mutex_);
    }

    bool tryLock() {
        return pthread_mutex_trylock(&mutex_) == 0;
    }

    pthread_mutex_t* get() {
        return &mutex_;
    }

private:
    pthread_mutex_t mutex_;

    friend class Condition;
};

/**
 * @brief 互斥锁守卫 (RAII)
 */
class MutexGuard : public NonCopyable {
public:
    explicit MutexGuard(Mutex& mutex)
        : mutex_(mutex) {
        mutex_.lock();
    }

    ~MutexGuard() {
        mutex_.unlock();
    }

private:
    Mutex& mutex_;
};

/**
 * @brief 读写锁类
 */
class RWLock : public NonCopyable {
public:
    RWLock() {
        pthread_rwlock_init(&rwlock_, nullptr);
    }

    ~RWLock() {
        pthread_rwlock_destroy(&rwlock_);
    }

    void readLock() {
        pthread_rwlock_rdlock(&rwlock_);
    }

    void writeLock() {
        pthread_rwlock_wrlock(&rwlock_);
    }

    void unlock() {
        pthread_rwlock_unlock(&rwlock_);
    }

    bool tryReadLock() {
        return pthread_rwlock_tryrdlock(&rwlock_) == 0;
    }

    bool tryWriteLock() {
        return pthread_rwlock_trywrlock(&rwlock_) == 0;
    }

private:
    pthread_rwlock_t rwlock_;
};

/**
 * @brief 读锁守卫
 */
class ReadLockGuard : public NonCopyable {
public:
    explicit ReadLockGuard(RWLock& lock)
        : lock_(lock) {
        lock_.readLock();
    }

    ~ReadLockGuard() {
        lock_.unlock();
    }

private:
    RWLock& lock_;
};

/**
 * @brief 写锁守卫
 */
class WriteLockGuard : public NonCopyable {
public:
    explicit WriteLockGuard(RWLock& lock)
        : lock_(lock) {
        lock_.writeLock();
    }

    ~WriteLockGuard() {
        lock_.unlock();
    }

private:
    RWLock& lock_;
};

// 便捷宏
#define SCOPED_LOCK(mutex) MutexGuard _guard_(mutex)
#define SCOPED_READ_LOCK(rwlock) ReadLockGuard _read_guard_(rwlock)
#define SCOPED_WRITE_LOCK(rwlock) WriteLockGuard _write_guard_(rwlock)

} // namespace legend