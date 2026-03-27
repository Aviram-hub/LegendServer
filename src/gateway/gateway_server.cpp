/**
 * @file gateway_server.cpp
 * @brief 网关服务器实现
 */

#include "gateway_server.h"
#include "core/reactor/channel.h"
#include "common/network/socket.h"
#include "common/network/address.h"
#include "common/network/connection.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

namespace legend {

GatewayServer::GatewayServer(EventLoop* loop, const GatewayConfig& config)
    : loop_(loop)
    , config_(config)
    , listenFd_(-1)
    , running_(false) {
}

GatewayServer::~GatewayServer() {
    stop();
}

bool GatewayServer::init() {
    // 创建监听socket
    listenFd_ = Socket::createNonBlockingTcpSocket();
    if (listenFd_ < 0) {
        return false;
    }

    // 设置SO_REUSEADDR和SO_REUSEPORT
    int opt = 1;
    setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(listenFd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    // 绑定地址
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(config_.port());
    inet_pton(AF_INET, config_.host().c_str(), &addr.sin_addr);

    if (bind(listenFd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(listenFd_);
        listenFd_ = -1;
        return false;
    }

    // 监听
    if (listen(listenFd_, 128) < 0) {
        close(listenFd_);
        listenFd_ = -1;
        return false;
    }

    return true;
}

bool GatewayServer::start() {
    if (running_) {
        return true;
    }

    // 创建Channel监听accept事件
    listenChannel_ = std::make_unique<Channel>(loop_, listenFd_);
    listenChannel_->setReadCallback([this](int64) {
        struct sockaddr_in peerAddr;
        socklen_t len = sizeof(peerAddr);
        int sockfd = accept(listenFd_, (struct sockaddr*)&peerAddr, &len);
        if (sockfd >= 0) {
            onNewConnection(sockfd, IPv4Address(peerAddr));
        }
    });
    listenChannel_->enableReading();

    running_ = true;
    return true;
}

void GatewayServer::stop() {
    if (!running_) {
        return;
    }

    running_ = false;

    // 关闭所有连接
    for (auto& pair : connections_) {
        pair.second->forceClose();
    }
    connections_.clear();

    // 关闭监听socket
    if (listenFd_ >= 0) {
        close(listenFd_);
        listenFd_ = -1;
    }

    listenChannel_.reset();
}

void GatewayServer::onNewConnection(int sockfd, const IPv4Address& peerAddr) {
    // 创建连接对象
    auto conn = std::make_shared<Connection>(loop_, sockfd,
        IPv4Address(socket_util::getLocalAddr(sockfd)), peerAddr);

    int64 connId = ++nextConnId_;
    connections_[connId] = conn;

    conn->setConnectionCallback([this](const Ptr<Connection>& c) {
        onConnectionClosed(c);
    });

    conn->setMessageCallback([this](const Ptr<Connection>& c, Buffer& buf, int64 time) {
        onMessage(c, buf, time);
    });

    conn->connectEstablished();
}

void GatewayServer::onConnectionClosed(const Ptr<Connection>& conn) {
    // 移除连接
    for (auto it = connections_.begin(); it != connections_.end(); ++it) {
        if (it->second.get() == conn.get()) {
            connections_.erase(it);
            break;
        }
    }
}

void GatewayServer::onMessage(const Ptr<Connection>& conn, Buffer& buffer, int64 receiveTime) {
    // 处理消息
    // 解析协议头，路由到对应处理器
}

} // namespace legend