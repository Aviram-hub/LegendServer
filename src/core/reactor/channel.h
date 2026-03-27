/**
 * @file channel.h
 * @brief 事件通道
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include <functional>

namespace legend {

class EventLoop;

/**
 * @brief 事件通道类
 * 封装文件描述符和事件处理
 */
class Channel : public NonCopyable {
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(int64)>;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    // 处理事件
    void handleEvent(int64 receiveTime);

    // 设置回调函数
    void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
    void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

    // 绑定对象，防止回调时对象已销毁
    void tie(const Ptr<void>& obj) { tie_ = obj; tied_ = true; }

    // 获取文件描述符
    int fd() const { return fd_; }

    // 获取事件
    int events() const { return events_; }

    // 设置返回的事件
    void setRevents(int revt) { revents_ = revt; }

    // 是否注册了可读事件
    bool isReading() const { return events_ & kReadEvent; }

    // 是否注册了可写事件
    bool isWriting() const { return events_ & kWriteEvent; }

    // 是否没有注册事件
    bool isNoneEvent() const { return events_ == kNoneEvent; }

    // 启用/禁用可读事件
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }

    // 启用/禁用可写事件
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }

    // 禁用所有事件
    void disableAll() { events_ = kNoneEvent; update(); }

    // 获取所属事件循环
    EventLoop* ownerLoop() { return loop_; }

    // 从EventLoop中移除
    void remove();

    // 获取索引（用于Poller）
    int index() const { return index_; }
    void setIndex(int idx) { index_ = idx; }

    // 事件类型
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

private:
    void update();
    void handleEventWithGuard(int64 receiveTime);

    EventLoop* loop_;
    int fd_;
    int events_;
    int revents_;
    int index_;

    bool tied_;
    bool eventHandling_;
    bool addedToLoop_;
    WeakPtr<void> tie_;

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

} // namespace legend