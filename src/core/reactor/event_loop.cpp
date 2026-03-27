/**
 * @file event_loop.cpp
 * @brief 事件循环实现
 */

#include "event_loop.h"
#include "epoll_poller.h"
#include "channel.h"
#include "common/timer/timer_manager.h"
#include <sys/eventfd.h>
#include <unistd.h>
#include <sys/syscall.h>

namespace legend {

namespace {
thread_local EventLoop* t_loopInThisThread = nullptr;
}

pid_t EventLoop::currentTid() {
    return syscall(SYS_gettid);
}

EventLoop::EventLoop()
    : looping_(false)
    , quit_(false)
    , eventHandling_(false)
    , callingPendingFunctors_(false)
    , threadId_(currentTid())
    , iteration_(0)
    , poller_(new EpollPoller(this))
    , timerManager_(new TimerManager())
    , wakeupFd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC))
    , wakeupChannel_(new Channel(this, wakeupFd_))
    , currentActiveChannel_(nullptr) {
    if (t_loopInThisThread) {
        // 已经有一个EventLoop在这个线程
    } else {
        t_loopInThisThread = this;
    }

    wakeupChannel_->setReadCallback([this](int64) { handleRead(); });
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop() {
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;

    while (!quit_) {
        activeChannels_.clear();
        int64 pollReturnTime = poller_->poll(10000, &activeChannels_);
        ++iteration_;

        eventHandling_ = true;
        for (Channel* channel : activeChannels_) {
            currentActiveChannel_ = channel;
            channel->handleEvent(pollReturnTime);
        }
        currentActiveChannel_ = nullptr;
        eventHandling_ = false;

        doPendingFunctors();

        // 处理定时器
        timerManager_->processExpiredTimers();
    }

    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;
    if (!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::runInLoop(Functor cb) {
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor cb) {
    {
        MutexGuard lock(mutex_);
        pendingFunctors_.push_back(std::move(cb));
    }

    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

TimerId EventLoop::runAt(int64 time, Functor cb) {
    return timerManager_->addTimer(time, std::move(cb), 0);
}

TimerId EventLoop::runAfter(double delay, Functor cb) {
    int64 time = nowMs() + static_cast<int64>(delay * 1000);
    return runAt(time, std::move(cb));
}

TimerId EventLoop::runEvery(double interval, Functor cb) {
    int64 time = nowMs() + static_cast<int64>(interval * 1000);
    return timerManager_->addTimer(time, std::move(cb),
                                    static_cast<int64>(interval * 1000));
}

void EventLoop::cancel(TimerId timerId) {
    timerManager_->cancel(timerId);
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof(one));
    (void)n;
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
    (void)n;
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        MutexGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (const Functor& functor : functors) {
        functor();
    }

    callingPendingFunctors_ = false;
}

void EventLoop::updateChannel(Channel* channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    if (eventHandling_) {
        assert(currentActiveChannel_ == channel ||
               std::find(activeChannels_.begin(), activeChannels_.end(), channel) ==
               activeChannels_.end());
    }
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    return poller_->hasChannel(channel);
}

void EventLoop::abortNotInLoopThread() {
    // 错误处理
}

EventLoop* EventLoop::getEventLoopOfCurrentThread() {
    return t_loopInThisThread;
}

} // namespace legend