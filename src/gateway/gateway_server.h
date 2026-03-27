/**
 * @file gateway_server.h
 * @brief 网关服务器
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "common/base/buffer.h"
#include "common/network/address.h"
#include "common/network/connection.h"
#include "gateway_config.h"
#include "core/reactor/event_loop.h"
#include <unordered_map>

namespace legend {

class Channel;

/**
 * @brief 网关服务器
 */
class GatewayServer : public NonCopyable {
public:
    GatewayServer(EventLoop* loop, const GatewayConfig& config);
    ~GatewayServer();

    bool init();
    bool start();
    void stop();

private:
    void onNewConnection(int sockfd, const IPv4Address& peerAddr);
    void onConnectionClosed(const Ptr<Connection>& conn);
    void onMessage(const Ptr<Connection>& conn, Buffer& buffer, int64 receiveTime);

    EventLoop* loop_;
    const GatewayConfig& config_;

    int listenFd_;
    UniquePtr<Channel> listenChannel_;

    HashMap<int64, Ptr<Connection>> connections_;
    std::atomic<int64> nextConnId_{0};

    std::atomic<bool> running_;
};

} // namespace legend