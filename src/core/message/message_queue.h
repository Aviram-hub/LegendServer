/**
 * @file message_queue.h
 * @brief 消息队列
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "common/thread/mutex.h"
#include "common/thread/condition.h"
#include <queue>

namespace legend {

class Message;

/**
 * @brief 消息队列项
 */
struct MessageItem {
    int64 connId;
    MessageId msgId;
    Ptr<Message> message;
    int64 timestamp;

    MessageItem()
        : connId(0)
        , msgId(MessageId::MSG_LOGIN_REQUEST)
        , timestamp(0) {
    }
};

/**
 * @brief 消息队列
 */
class MessageQueue : public NonCopyable {
public:
    MessageQueue() = default;
    ~MessageQueue() = default;

    // 推入消息
    void push(const MessageItem& item) {
        MutexGuard guard(mutex_);
        queue_.push(item);
        notEmpty_.notify();
    }

    void push(MessageItem&& item) {
        MutexGuard guard(mutex_);
        queue_.push(std::move(item));
        notEmpty_.notify();
    }

    // 弹出消息
    bool pop(MessageItem& item, uint32 timeoutMs = 0) {
        MutexGuard guard(mutex_);

        while (queue_.empty() && !stopped_) {
            if (timeoutMs == 0) {
                notEmpty_.wait(mutex_);
            } else {
                if (!notEmpty_.waitFor(mutex_, timeoutMs)) {
                    return false;
                }
            }
        }

        if (stopped_ || queue_.empty()) {
            return false;
        }

        item = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    // 批量弹出
    void popAll(std::queue<MessageItem>& out) {
        MutexGuard guard(mutex_);
        out = std::move(queue_);
    }

    // 队列是否为空
    bool empty() const {
        MutexGuard guard(mutex_);
        return queue_.empty();
    }

    // 获取队列大小
    size_t size() const {
        MutexGuard guard(mutex_);
        return queue_.size();
    }

    // 停止队列
    void stop() {
        MutexGuard guard(mutex_);
        stopped_ = true;
        notEmpty_.notifyAll();
    }

    // 重置
    void reset() {
        MutexGuard guard(mutex_);
        stopped_ = false;
        while (!queue_.empty()) {
            queue_.pop();
        }
    }

private:
    mutable Mutex mutex_;
    Condition notEmpty_;
    std::queue<MessageItem> queue_;
    bool stopped_ = false;
};

} // namespace legend