/**
 * @file event_loop.h
 * @brief 事件循环
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "common/thread/mutex.h"
#include "common/timer/timer_manager.h"
#include <atomic>
#include <functional>
#include <vector>

namespace legend {

class Channel;
class EpollPoller;
class Timer;

/**
 * @brief 事件循环类
 * Reactor模式的核心
 */
class EventLoop : public NonCopyable {
public:
    using Functor = std::function<void()>;
    using ChannelList = std::vector<Channel*>;

    EventLoop();
    ~EventLoop();

    // 事件循环
    void loop();

    // 退出事件循环
    void quit();

    // 在事件循环中运行
    void runInLoop(Functor cb);

    // 投递到事件循环
    void queueInLoop(Functor cb);

    // 定时器相关
    TimerId runAt(int64 time, Functor cb);
    TimerId runAfter(double delay, Functor cb);
    TimerId runEvery(double interval, Functor cb);
    void cancel(TimerId timerId);

    // 在事件循环内调用
    void wakeup();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    // 断言在事件循环线程
    void assertInLoopThread() {
        if (!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    // 是否在事件循环线程
    bool isInLoopThread() const {
        return threadId_ == currentTid();
    }

    // 获取事件循环
    static EventLoop* getEventLoopOfCurrentThread();

    // 当前线程ID
    pid_t threadId() const { return threadId_; }

private:
    void abortNotInLoopThread();
    void handleRead();
    void doPendingFunctors();

    void printActiveChannels() const;

    std::atomic<bool> looping_;
    std::atomic<bool> quit_;
    std::atomic<bool> eventHandling_;
    std::atomic<bool> callingPendingFunctors_;

    const pid_t threadId_;
    int64 iteration_;

    UniquePtr<EpollPoller> poller_;
    UniquePtr<TimerManager> timerManager_;

    int wakeupFd_;
    UniquePtr<Channel> wakeupChannel_;

    ChannelList activeChannels_;
    Channel* currentActiveChannel_;

    mutable Mutex mutex_;
    std::vector<Functor> pendingFunctors_;

    static pid_t currentTid();
};

} // namespace legend