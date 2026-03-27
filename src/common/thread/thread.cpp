/**
 * @file thread.cpp
 * @brief 线程实现
 */

#include "thread.h"
#include "condition.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <cstring>
#include <cassert>

namespace legend {

std::atomic<int32> Thread::numCreated_(0);

namespace {
// 线程局部存储
thread_local pid_t t_tid = 0;
thread_local const char* t_threadName = "unknown";

// 线程数据结构
struct ThreadData {
    Thread::ThreadFunc func;
    String name;
    pid_t* tid;
    CountDownLatch* latch;

    void run() {
        t_tid = syscall(SYS_gettid);
        t_threadName = name.empty() ? "thread" : name.c_str();

        *tid = t_tid;
        latch->countDown();
        latch = nullptr;

        try {
            func();
        } catch (...) {
            // 异常处理
        }

        t_threadName = "finished";
    }
};

} // anonymous namespace

Thread::Thread(ThreadFunc func, const String& name)
    : started_(false)
    , joined_(false)
    , pthreadId_(0)
    , tid_(0)
    , func_(std::move(func))
    , name_(name) {
    setDefaultName();
}

Thread::~Thread() {
    if (started_ && !joined_) {
        pthread_detach(pthreadId_);
    }
}

Thread::Thread(Thread&& other) noexcept
    : started_(other.started_)
    , joined_(other.joined_)
    , pthreadId_(other.pthreadId_)
    , tid_(other.tid_)
    , func_(std::move(other.func_))
    , name_(std::move(other.name_)) {
    other.started_ = false;
    other.joined_ = false;
    other.pthreadId_ = 0;
    other.tid_ = 0;
}

Thread& Thread::operator=(Thread&& other) noexcept {
    if (this != &other) {
        if (started_ && !joined_) {
            pthread_detach(pthreadId_);
        }
        started_ = other.started_;
        joined_ = other.joined_;
        pthreadId_ = other.pthreadId_;
        tid_ = other.tid_;
        func_ = std::move(other.func_);
        name_ = std::move(other.name_);
        other.started_ = false;
        other.joined_ = false;
        other.pthreadId_ = 0;
        other.tid_ = 0;
    }
    return *this;
}

void Thread::start() {
    assert(!started_);
    started_ = true;

    auto* data = new ThreadData;
    data->func = func_;
    data->name = name_;
    data->tid = &tid_;

    auto latch = makePtr<CountDownLatch>(1);
    data->latch = latch.get();

    if (pthread_create(&pthreadId_, nullptr, threadRoutine, data) != 0) {
        started_ = false;
        delete data;
        // 错误处理
    } else {
        latch->wait();
    }
}

void Thread::join() {
    assert(started_);
    assert(!joined_);
    joined_ = true;
    pthread_join(pthreadId_, nullptr);
}

void Thread::setDefaultName() {
    if (name_.empty()) {
        name_ = "Thread-" + std::to_string(++numCreated_);
    }
}

void* Thread::threadRoutine(void* arg) {
    auto* data = static_cast<ThreadData*>(arg);
    data->run();
    delete data;
    return nullptr;
}

pid_t Thread::currentTid() {
    if (t_tid == 0) {
        t_tid = syscall(SYS_gettid);
    }
    return t_tid;
}

const char* Thread::currentName() {
    return t_threadName;
}

bool Thread::isMainThread() {
    return currentTid() == getpid();
}

} // namespace legend