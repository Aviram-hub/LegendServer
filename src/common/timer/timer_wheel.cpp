/**
 * @file timer_wheel.cpp
 * @brief 时间轮定时器实现
 */

#include "timer_wheel.h"

namespace legend {

TimerWheel::TimerWheel(uint32 tickMs, uint32 wheelSize)
    : tickMs_(tickMs)
    , wheelSize_(wheelSize)
    , currentSlot_(0)
    , currentTime_(nowMs())
    , wheel_(wheelSize)
    , nextTimerId_(1) {
}

TimerWheel::~TimerWheel() {
}

TimerWheel::TimerId TimerWheel::addTimer(uint32 timeoutMs, TimerCallback cb) {
    MutexGuard guard(mutex_);

    TimerId id = generateTimerId();
    uint32 ticks = timeoutMs / tickMs_;
    if (timeoutMs < tickMs_) {
        ticks = 1;
    }

    uint32 rounds = ticks / wheelSize_;
    uint32 slot = (currentSlot_ + ticks) % wheelSize_;

    TimerEntry entry;
    entry.id = id;
    entry.rounds = rounds;
    entry.callback = std::move(cb);

    wheel_[slot].push_back(entry);
    auto it = wheel_[slot].end();
    --it;
    timerMap_[id] = std::make_pair(slot, it);

    return id;
}

bool TimerWheel::cancel(TimerId timerId) {
    MutexGuard guard(mutex_);

    auto it = timerMap_.find(timerId);
    if (it == timerMap_.end()) {
        return false;
    }

    uint32 slot = it->second.first;
    wheel_[slot].erase(it->second.second);
    timerMap_.erase(it);

    return true;
}

void TimerWheel::tick() {
    MutexGuard guard(mutex_);

    currentTime_ += tickMs_;

    for (auto it = wheel_[currentSlot_].begin(); it != wheel_[currentSlot_].end(); ) {
        if (it->rounds > 0) {
            --(it->rounds);
            ++it;
        } else {
            TimerCallback cb = std::move(it->callback);
            TimerId id = it->id;
            timerMap_.erase(id);
            it = wheel_[currentSlot_].erase(it);

            // 执行回调
            if (cb) {
                cb();
            }
        }
    }

    currentSlot_ = (currentSlot_ + 1) % wheelSize_;
}

TimerWheel::TimerId TimerWheel::generateTimerId() {
    return nextTimerId_++;
}

} // namespace legend