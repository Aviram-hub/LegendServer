/**
 * @file timer.h
 * @brief 定时器
 */

#pragma once

#include "types.h"
#include "noncopyable.h"
#include <functional>

namespace legend {

/**
 * @brief 定时器类
 */
class Timer : public NonCopyable {
public:
    using TimerCallback = std::function<void()>;

    Timer(TimerCallback cb, int64 when, int64 interval)
        : callback_(std::move(cb))
        , expiration_(when)
        , interval_(interval)
        , repeat_(interval > 0)
        , sequence_(++s_numCreated_) {
    }

    void run() const {
        if (callback_) {
            callback_();
        }
    }

    int64 expiration() const { return expiration_; }
    bool repeat() const { return repeat_; }
    int64 sequence() const { return sequence_; }

    void restart(int64 now) {
        if (repeat_) {
            expiration_ = now + interval_;
        } else {
            expiration_ = 0;
        }
    }

    static int64 numCreated() { return s_numCreated_.load(); }

private:
    TimerCallback callback_;
    int64 expiration_;
    const int64 interval_;
    const bool repeat_;
    const int64 sequence_;

    static std::atomic<int64> s_numCreated_;
};

/**
 * @brief 定时器比较器
 */
struct TimerComparator {
    bool operator()(const Timer* lhs, const Timer* rhs) const {
        if (lhs->expiration() != rhs->expiration()) {
            return lhs->expiration() < rhs->expiration();
        }
        return lhs->sequence() < rhs->sequence();
    }
};

} // namespace legend