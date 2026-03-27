/**
 * @file timer_manager.h
 * @brief 定时器管理器
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "timer.h"
#include "common/thread/mutex.h"
#include <set>
#include <vector>

namespace legend {

class Timer;

/**
 * @brief 定时器ID
 */
struct TimerId {
    TimerId() : timer(nullptr), sequence(0) {}
    TimerId(Timer* t, int64 seq) : timer(t), sequence(seq) {}

    Timer* timer;
    int64 sequence;
};

/**
 * @brief 定时器管理器
 */
class TimerManager : public NonCopyable {
public:
    TimerManager() = default;
    ~TimerManager();

    // 添加定时器
    TimerId addTimer(int64 when, Timer::TimerCallback cb, int64 interval = 0);

    // 取消定时器
    void cancel(TimerId timerId);

    // 处理过期定时器
    void processExpiredTimers();

    // 获取最近一个定时器的过期时间
    int64 getNextExpiration() const;

    // 是否有定时器
    bool hasTimer() const { return !timers_.empty(); }

private:
    using TimerList = std::set<Timer*, TimerComparator>;
    using ActiveTimerSet = std::set<std::pair<Timer*, int64>>;

    void addTimerInLoop(Timer* timer);
    void cancelInLoop(TimerId timerId);

    // 获取过期定时器
    std::vector<Timer*> getExpiredTimers(int64 now);

    // 重置重复定时器
    void resetTimers(const std::vector<Timer*>& expired, int64 now);

    TimerList timers_;
    ActiveTimerSet activeTimers_;
    std::atomic<bool> callingExpiredTimers_;
    ActiveTimerSet cancelingTimers_;
};

} // namespace legend