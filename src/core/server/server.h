/**
 * @file server.h
 * @brief 服务器基类
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "common/base/buffer.h"
#include "server_config.h"
#include "common/network/address.h"
#include "common/thread/thread_pool.h"
#include <unordered_map>

namespace legend {

class EventLoop;
class Connection;
class Session;

/**
 * @brief 服务器类
 */
class Server : public NonCopyable {
public:
    Server(EventLoop* loop, const ServerConfig& config);
    ~Server();

    // 初始化
    bool init();

    // 启动服务器
    bool start();

    // 停止服务器
    void stop();

    // 获取配置
    const ServerConfig& config() const { return config_; }

    // 获取事件循环
    EventLoop* loop() { return loop_; }

    // 获取线程池
    ThreadPool* threadPool() { return threadPool_.get(); }

private:
    void onNewConnection(int sockfd, const IPv4Address& peerAddr);
    void onConnectionClosed(const Ptr<Connection>& conn);
    void onMessage(const Ptr<Connection>& conn, Buffer& buffer, int64 receiveTime);

    EventLoop* loop_;
    const ServerConfig& config_;

    UniquePtr<ThreadPool> threadPool_;

    int gatewayFd_;
    int gameFd_;

    std::atomic<bool> running_;
};

} // namespace legend