/**
 * @file timer_manager.cpp
 * @brief 定时器管理器实现
 */

#include "timer_manager.h"
#include <algorithm>

namespace legend {

TimerManager::~TimerManager() {
    for (Timer* timer : timers_) {
        delete timer;
    }
}

TimerId TimerManager::addTimer(int64 when, Timer::TimerCallback cb, int64 interval) {
    Timer* timer = new Timer(std::move(cb), when, interval);
    addTimerInLoop(timer);
    return TimerId(timer, timer->sequence());
}

void TimerManager::cancel(TimerId timerId) {
    cancelInLoop(timerId);
}

void TimerManager::processExpiredTimers() {
    std::vector<Timer*> expired = getExpiredTimers(nowMs());

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();

    for (Timer* timer : expired) {
        timer->run();
    }
    callingExpiredTimers_ = false;

    resetTimers(expired, nowMs());
}

int64 TimerManager::getNextExpiration() const {
    if (timers_.empty()) {
        return -1;
    }
    return (*timers_.begin())->expiration();
}

void TimerManager::addTimerInLoop(Timer* timer) {
    bool earliestChanged = false;
    int64 when = timer->expiration();

    auto it = timers_.begin();
    if (it == timers_.end() || when < (*it)->expiration()) {
        earliestChanged = true;
    }

    timers_.insert(timer);
    activeTimers_.insert(std::make_pair(timer, timer->sequence()));
}

void TimerManager::cancelInLoop(TimerId timerId) {
    auto it = activeTimers_.find(std::make_pair(timerId.timer, timerId.sequence));
    if (it != activeTimers_.end()) {
        size_t n = timers_.erase(it->first);
        delete it->first;
        activeTimers_.erase(it);
    } else if (callingExpiredTimers_) {
        cancelingTimers_.insert(std::make_pair(timerId.timer, timerId.sequence));
    }
}

std::vector<Timer*> TimerManager::getExpiredTimers(int64 now) {
    std::vector<Timer*> expired;

    Timer sentry(nullptr, now, 0);
    auto end = timers_.lower_bound(&sentry);
    for (auto it = timers_.begin(); it != end; ++it) {
        expired.push_back(*it);
    }

    timers_.erase(timers_.begin(), end);
    for (Timer* timer : expired) {
        activeTimers_.erase(std::make_pair(timer, timer->sequence()));
    }

    return expired;
}

void TimerManager::resetTimers(const std::vector<Timer*>& expired, int64 now) {
    for (Timer* timer : expired) {
        auto it = cancelingTimers_.find(std::make_pair(timer, timer->sequence()));
        if (it == cancelingTimers_.end() && timer->repeat()) {
            timer->restart(now);
            addTimerInLoop(timer);
        } else {
            delete timer;
        }
    }
    cancelingTimers_.clear();
}

} // namespace legend