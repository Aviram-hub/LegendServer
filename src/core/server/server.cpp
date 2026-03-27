/**
 * @file server.cpp
 * @brief 服务器实现
 */

#include "server.h"
#include "server_config.h"
#include "core/reactor/event_loop.h"
#include "core/session/session_manager.h"
#include "common/network/socket.h"
#include "common/network/connection.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace legend {

Server::Server(EventLoop* loop, const ServerConfig& config)
    : loop_(loop)
    , config_(config)
    , gatewayFd_(-1)
    , gameFd_(-1)
    , running_(false) {
}

Server::~Server() {
    stop();
}

bool Server::init() {
    // 初始化线程池
    threadPool_ = std::make_unique<ThreadPool>("LegendServer-Pool");
    threadPool_->start(config_.ioThreads() + config_.logicThreads());

    // 初始化数据库连接池
    // ...

    // 初始化Lua引擎
    // ...

    return true;
}

bool Server::start() {
    if (running_) {
        return true;
    }

    // 创建网关监听socket
    gatewayFd_ = Socket::createNonBlockingTcpSocket();
    if (gatewayFd_ < 0) {
        return false;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(config_.gatewayPort());
    inet_pton(AF_INET, config_.gatewayHost().c_str(), &addr.sin_addr);

    int opt = 1;
    setsockopt(gatewayFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(gatewayFd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    if (bind(gatewayFd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(gatewayFd_);
        gatewayFd_ = -1;
        return false;
    }

    if (listen(gatewayFd_, 128) < 0) {
        close(gatewayFd_);
        gatewayFd_ = -1;
        return false;
    }

    // 创建游戏服务器监听socket
    gameFd_ = Socket::createNonBlockingTcpSocket();
    if (gameFd_ < 0) {
        close(gatewayFd_);
        return false;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(config_.gamePort());
    inet_pton(AF_INET, config_.gameHost().c_str(), &addr.sin_addr);

    setsockopt(gameFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(gameFd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    if (bind(gameFd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(gatewayFd_);
        close(gameFd_);
        return false;
    }

    if (listen(gameFd_, 128) < 0) {
        close(gatewayFd_);
        close(gameFd_);
        return false;
    }

    running_ = true;
    return true;
}

void Server::stop() {
    if (!running_) {
        return;
    }

    running_ = false;

    // 关闭监听socket
    if (gatewayFd_ >= 0) {
        close(gatewayFd_);
        gatewayFd_ = -1;
    }

    if (gameFd_ >= 0) {
        close(gameFd_);
        gameFd_ = -1;
    }

    // 停止线程池
    if (threadPool_) {
        threadPool_->stop();
    }
}

void Server::onNewConnection(int sockfd, const IPv4Address& peerAddr) {
    // 创建会话
    auto session = SessionManager::instance().createSession();
    if (session) {
        session->setLoginIp(peerAddr.ip());
    }
}

void Server::onConnectionClosed(const Ptr<Connection>& conn) {
    // 清理会话
}

void Server::onMessage(const Ptr<Connection>& conn, Buffer& buffer, int64 receiveTime) {
    // 处理消息
}

} // namespace legend