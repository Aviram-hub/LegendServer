/**
 * @file client_connection.h
 * @brief 客户端连接管理
 */

#pragma once

#include "types.h"
#include "noncopyable.h"
#include "common/network/connection.h"

namespace legend {

/**
 * @brief 客户端连接
 */
class ClientConnection : public NonCopyable {
public:
    ClientConnection(int64 id, Ptr<Connection> conn);
    ~ClientConnection();

    int64 id() const { return id_; }
    Ptr<Connection> connection() const { return connection_; }

    // 玩家ID
    int64 playerId() const { return playerId_; }
    void setPlayerId(int64 id) { playerId_ = id; }

    // 角色ID
    int64 roleId() const { return roleId_; }
    void setRoleId(int64 id) { roleId_ = id; }

    // 发送消息
    void send(const void* data, size_t len);
    void close();

    // 心跳
    void updateHeartbeat() { lastHeartbeat_ = nowMs(); }
    bool isHeartbeatTimeout(int64 timeoutMs) const {
        return nowMs() - lastHeartbeat_ > timeoutMs;
    }

private:
    int64 id_;
    Ptr<Connection> connection_;
    int64 playerId_{0};
    int64 roleId_{0};
    int64 lastHeartbeat_{0};
};

} // namespace legend