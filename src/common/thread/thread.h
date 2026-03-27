/**
 * @file thread.h
 * @brief 线程封装
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include <pthread.h>
#include <functional>
#include <string>
#include <atomic>

namespace legend {

/**
 * @brief 线程类
 */
class Thread : public NonCopyable {
public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(ThreadFunc func, const String& name = "");
    ~Thread();

    Thread(Thread&& other) noexcept;
    Thread& operator=(Thread&& other) noexcept;

    // 启动线程
    void start();

    // 等待线程结束
    void join();

    // 线程是否已启动
    bool started() const { return started_; }

    // 线程是否已加入
    bool joined() const { return joined_; }

    // 获取线程ID
    pid_t tid() const { return tid_; }

    // 获取线程名称
    const String& name() const { return name_; }

    // 设置线程名称
    void setName(const String& name) { name_ = name; }

    // 获取当前线程ID
    static pid_t currentTid();

    // 获取当前线程名称
    static const char* currentName();

    // 当前线程是否是主线程
    static bool isMainThread();

private:
    void setDefaultName();

    static void* threadRoutine(void* arg);

    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    pid_t tid_;
    ThreadFunc func_;
    String name_;

    static std::atomic<int32> numCreated_;
};

} // namespace legend