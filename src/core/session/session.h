/**
 * @file session.h
 * @brief 会话类
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "common/network/connection.h"
#include <memory>

namespace legend {

// 前向声明
class Message;

/**
 * @brief 会话状态
 */
enum class SessionState {
    kNone,          // 无状态
    kConnecting,    // 连接中
    kConnected,     // 已连接
    kLogined,       // 已登录
    kPlaying,       // 游戏中
    kClosing,       // 关闭中
    kClosed         // 已关闭
};

/**
 * @brief 会话类
 */
class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(int64 id);
    ~Session();

    // 会话ID
    int64 id() const { return id_; }

    // 玩家ID
    int64 playerId() const { return playerId_; }
    void setPlayerId(int64 id) { playerId_ = id; }

    // 角色ID
    int64 roleId() const { return roleId_; }
    void setRoleId(int64 id) { roleId_ = id; }

    // 会话状态
    SessionState state() const { return state_; }
    void setState(SessionState state) { state_ = state; }

    // 连接
    Ptr<Connection> connection() const { return connection_; }
    void setConnection(Ptr<Connection> conn) { connection_ = conn; }

    // 发送消息
    void send(const Message& msg);
    void send(const void* data, size_t len);

    // 关闭会话
    void close();

    // 心跳相关
    void updateHeartbeat() { lastHeartbeat_ = nowMs(); }
    int64 lastHeartbeat() const { return lastHeartbeat_; }
    bool isHeartbeatTimeout(int64 timeoutMs) const {
        return nowMs() - lastHeartbeat_ > timeoutMs;
    }

    // 认证Token
    const String& token() const { return token_; }
    void setToken(const String& token) { token_ = token; }

    // 登录时间
    int64 loginTime() const { return loginTime_; }
    void setLoginTime(int64 time) { loginTime_ = time; }

    // 登录IP
    const String& loginIp() const { return loginIp_; }
    void setLoginIp(const String& ip) { loginIp_ = ip; }

    // 上下文数据
    void setContext(const Ptr<void>& ctx) { context_ = ctx; }
    Ptr<void> getContext() const { return context_; }

private:
    int64 id_;
    int64 playerId_{0};
    int64 roleId_{0};
    SessionState state_{SessionState::kNone};

    Ptr<Connection> connection_;
    String token_;
    int64 lastHeartbeat_{0};
    int64 loginTime_{0};
    String loginIp_;

    Ptr<void> context_;
};

} // namespace legend