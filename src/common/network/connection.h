/**
 * @file connection.h
 * @brief TCP连接封装
 */

#pragma once

#include "types.h"
#include "noncopyable.h"
#include "buffer.h"
#include "socket.h"
#include "address.h"
#include <functional>

namespace legend {

class EventLoop;
class Channel;

/**
 * @brief TCP连接类
 */
class Connection : public std::enable_shared_from_this<Connection> {
public:
    using ConnectionCallback = std::function<void(const Ptr<Connection>&)>;
    using MessageCallback = std::function<void(const Ptr<Connection>&, Buffer&, int64)>;
    using WriteCompleteCallback = std::function<void(const Ptr<Connection>&)>;
    using HighWaterMarkCallback = std::function<void(const Ptr<Connection>&, size_t)>;

    Connection(EventLoop* loop, int sockfd, const IPv4Address& localAddr, const IPv4Address& peerAddr);
    ~Connection();

    // 获取所属事件循环
    EventLoop* loop() const { return loop_; }

    // 获取socket文件描述符
    int fd() const { return socket_->fd(); }

    // 获取socket
    Socket* socket() { return socket_.get(); }

    // 获取本地地址
    const IPv4Address& localAddress() const { return localAddr_; }

    // 获取对端地址
    const IPv4Address& peerAddress() const { return peerAddr_; }

    // 获取连接ID
    int64 id() const { return id_; }

    // 连接名称
    String name() const;

    // 发送数据
    void send(const void* data, size_t len);
    void send(const String& data);
    void send(Buffer& buffer);

    // 关闭连接
    void shutdown();
    void forceClose();
    void forceCloseWithDelay(int64 milliseconds);

    // 设置TCP_NODELAY
    void setTcpNoDelay(bool on = true);

    // 连接是否已建立
    bool connected() const { return state_ == kConnected; }

    // 连接是否已断开
    bool disconnected() const { return state_ == kDisconnected; }

    // 获取输入缓冲区
    Buffer* inputBuffer() { return &inputBuffer_; }
    const Buffer& inputBuffer() const { return inputBuffer_; }

    // 获取输出缓冲区
    Buffer* outputBuffer() { return &outputBuffer_; }
    const Buffer& outputBuffer() const { return outputBuffer_; }

    // 设置回调函数
    void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }
    void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark) {
        highWaterMarkCallback_ = cb;
        highWaterMark_ = highWaterMark;
    }

    // 设置上下文数据
    void setContext(const Ptr<void>& context) { context_ = context; }
    Ptr<void> getContext() const { return context_; }

    // 连接建立
    void connectEstablished();

    // 连接销毁
    void connectDestroyed();

private:
    enum State { kDisconnected, kConnecting, kConnected, kDisconnecting };

    void setState(State state) { state_ = state; }

    void handleRead(int64 receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const void* data, size_t len);
    void sendInLoop(const String& data);
    void shutdownInLoop();
    void forceCloseInLoop();

    static std::atomic<int64> nextId_;

    EventLoop* loop_;
    UniquePtr<Socket> socket_;
    UniquePtr<Channel> channel_;
    IPv4Address localAddr_;
    IPv4Address peerAddr_;
    int64 id_;
    State state_;

    Buffer inputBuffer_;
    Buffer outputBuffer_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    HighWaterMarkCallback highWaterMarkCallback_;
    size_t highWaterMark_;

    Ptr<void> context_;
};

} // namespace legend