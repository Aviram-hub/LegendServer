/**
 * @file timer_wheel.h
 * @brief 时间轮定时器实现
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "common/thread/mutex.h"
#include <vector>
#include <list>
#include <functional>

namespace legend {

/**
 * @brief 时间轮定时器
 * 高效处理大量定时任务
 */
class TimerWheel : public NonCopyable {
public:
    using TimerCallback = std::function<void()>;
    using TimerId = uint64;

    /**
     * @brief 构造函数
     * @param tickMs 时间刻度(毫秒)
     * @param wheelSize 轮大小
     */
    TimerWheel(uint32 tickMs = 100, uint32 wheelSize = 60);
    ~TimerWheel();

    // 添加定时器
    TimerId addTimer(uint32 timeoutMs, TimerCallback cb);

    // 取消定时器
    bool cancel(TimerId timerId);

    // 推进时间轮
    void tick();

    // 获取当前时间
    int64 currentTime() const { return currentTime_; }

private:
    struct TimerEntry {
        TimerId id;
        uint32 rounds;
        TimerCallback callback;
    };

    using TimerSlot = std::list<TimerEntry>;

    TimerId generateTimerId();

    uint32 tickMs_;
    uint32 wheelSize_;
    uint32 currentSlot_;
    int64 currentTime_;

    std::vector<TimerSlot> wheel_;
    HashMap<TimerId, std::pair<uint32, TimerSlot::iterator>> timerMap_;

    std::atomic<TimerId> nextTimerId_;
    mutable Mutex mutex_;
};

} // namespace legend