/**
 * @file connection.cpp
 * @brief TCP连接实现
 */

#include "connection.h"
#include "core/reactor/event_loop.h"
#include "core/reactor/channel.h"
#include <sys/uio.h>

namespace legend {

std::atomic<int64> Connection::nextId_(0);

Connection::Connection(EventLoop* loop, int sockfd,
                       const IPv4Address& localAddr,
                       const IPv4Address& peerAddr)
    : loop_(loop)
    , socket_(new Socket(sockfd))
    , channel_(new Channel(loop, sockfd))
    , localAddr_(localAddr)
    , peerAddr_(peerAddr)
    , id_(++nextId_)
    , state_(kConnecting)
    , highWaterMark_(64 * 1024 * 1024) {  // 64MB
    channel_->setReadCallback(
        [this](int64 receiveTime) { handleRead(receiveTime); });
    channel_->setWriteCallback(
        [this]() { handleWrite(); });
    channel_->setCloseCallback(
        [this]() { handleClose(); });
    channel_->setErrorCallback(
        [this]() { handleError(); });

    socket_->setKeepAlive(true);
}

Connection::~Connection() {
    assert(state_ == kDisconnected);
}

String Connection::name() const {
    return peerAddr_.ipPort() + "#" + std::to_string(id_);
}

void Connection::send(const void* data, size_t len) {
    if (state_ == kConnected) {
        if (loop_->isInLoopThread()) {
            sendInLoop(data, len);
        } else {
            String message(static_cast<const char*>(data), len);
            loop_->runInLoop([this, message]() {
                sendInLoop(message);
            });
        }
    }
}

void Connection::send(const String& data) {
    send(data.data(), data.size());
}

void Connection::send(Buffer& buffer) {
    if (state_ == kConnected) {
        if (loop_->isInLoopThread()) {
            sendInLoop(buffer.peek(), buffer.readableBytes());
            buffer.retrieveAll();
        } else {
            String message = buffer.retrieveAllAsString();
            loop_->runInLoop([this, message]() {
                sendInLoop(message);
            });
        }
    }
}

void Connection::shutdown() {
    if (state_ == kConnected) {
        setState(kDisconnecting);
        loop_->runInLoop([this]() { shutdownInLoop(); });
    }
}

void Connection::forceClose() {
    if (state_ == kConnected || state_ == kDisconnecting) {
        setState(kDisconnecting);
        loop_->queueInLoop([this]() { forceCloseInLoop(); });
    }
}

void Connection::forceCloseWithDelay(int64 milliseconds) {
    if (state_ == kConnected || state_ == kDisconnecting) {
        setState(kDisconnecting);
        loop_->runAfter(milliseconds / 1000.0, [this]() { forceCloseInLoop(); });
    }
}

void Connection::setTcpNoDelay(bool on) {
    socket_->setTcpNoDelay(on);
}

void Connection::connectEstablished() {
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    channel_->enableReading();

    if (connectionCallback_) {
        connectionCallback_(shared_from_this());
    }
}

void Connection::connectDestroyed() {
    loop_->assertInLoopThread();
    if (state_ == kConnected) {
        setState(kDisconnected);
        channel_->disableAll();
        if (connectionCallback_) {
            connectionCallback_(shared_from_this());
        }
    }
    channel_->remove();
}

void Connection::handleRead(int64 receiveTime) {
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(fd(), &savedErrno);
    if (n > 0) {
        if (messageCallback_) {
            messageCallback_(shared_from_this(), inputBuffer_, receiveTime);
        }
    } else if (n == 0) {
        handleClose();
    } else {
        errno = savedErrno;
        handleError();
    }
}

void Connection::handleWrite() {
    loop_->assertInLoopThread();
    if (channel_->isWriting()) {
        ssize_t n = ::write(fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
        if (n > 0) {
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readableBytes() == 0) {
                channel_->disableWriting();
                if (writeCompleteCallback_) {
                    loop_->queueInLoop([this]() {
                        writeCompleteCallback_(shared_from_this());
                    });
                }
                if (state_ == kDisconnecting) {
                    shutdownInLoop();
                }
            }
        }
    }
}

void Connection::handleClose() {
    loop_->assertInLoopThread();
    assert(state_ == kConnected || state_ == kDisconnecting);
    setState(kDisconnected);
    channel_->disableAll();

    Ptr<Connection> guardThis(shared_from_this());
    if (connectionCallback_) {
        connectionCallback_(guardThis);
    }
    // 通知服务器关闭连接
}

void Connection::handleError() {
    int err = socket_->getSocketError();
    // 错误处理
}

void Connection::sendInLoop(const void* data, size_t len) {
    loop_->assertInLoopThread();
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;

    if (state_ == kDisconnected) {
        return;
    }

    // 如果没有待发送数据，直接写
    if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
        nwrote = ::write(fd(), data, len);
        if (nwrote >= 0) {
            remaining = len - nwrote;
            if (remaining == 0 && writeCompleteCallback_) {
                loop_->queueInLoop([this]() {
                    writeCompleteCallback_(shared_from_this());
                });
            }
        } else {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                if (errno == EPIPE || errno == ECONNRESET) {
                    faultError = true;
                }
            }
        }
    }

    if (!faultError && remaining > 0) {
        size_t oldLen = outputBuffer_.readableBytes();
        if (oldLen + remaining >= highWaterMark_ &&
            oldLen < highWaterMark_ && highWaterMarkCallback_) {
            loop_->queueInLoop([this, oldLen, remaining]() {
                highWaterMarkCallback_(shared_from_this(), oldLen + remaining);
            });
        }
        outputBuffer_.append(static_cast<const char*>(data) + nwrote, remaining);
        if (!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}

void Connection::sendInLoop(const String& data) {
    sendInLoop(data.data(), data.size());
}

void Connection::shutdownInLoop() {
    loop_->assertInLoopThread();
    if (!channel_->isWriting()) {
        socket_->shutdownWrite();
    }
}

void Connection::forceCloseInLoop() {
    loop_->assertInLoopThread();
    if (state_ == kConnected || state_ == kDisconnecting) {
        handleClose();
    }
}

} // namespace legend