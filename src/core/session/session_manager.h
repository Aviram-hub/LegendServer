/**
 * @file session_manager.h
 * @brief 会话管理器
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "session.h"
#include "common/thread/mutex.h"
#include <unordered_map>

namespace legend {

// 前向声明
class Message;

/**
 * @brief 会话管理器
 */
class SessionManager : public NonCopyable {
public:
    static SessionManager& instance();

    // 创建会话
    Ptr<Session> createSession();

    // 获取会话
    Ptr<Session> getSession(int64 sessionId);

    // 通过玩家ID获取会话
    Ptr<Session> getSessionByPlayerId(int64 playerId);

    // 通过角色ID获取会话
    Ptr<Session> getSessionByRoleId(int64 roleId);

    // 移除会话
    void removeSession(int64 sessionId);

    // 绑定玩家
    bool bindPlayer(int64 sessionId, int64 playerId);

    // 绑定角色
    bool bindRole(int64 sessionId, int64 roleId);

    // 解绑玩家
    void unbindPlayer(int64 playerId);

    // 解绑角色
    void unbindRole(int64 roleId);

    // 获取会话数量
    size_t sessionCount() const;

    // 获取在线玩家数量
    size_t onlinePlayerCount() const;

    // 检查会话是否存在
    bool hasSession(int64 sessionId) const;

    // 检查玩家是否在线
    bool isPlayerOnline(int64 playerId) const;

    // 遍历所有会话
    template<typename Func>
    void foreachSession(Func func) {
        MutexGuard guard(mutex_);
        for (auto& pair : sessions_) {
            func(pair.second);
        }
    }

    // 广播消息
    void broadcast(const Message& msg);

private:
    SessionManager() = default;

    mutable Mutex mutex_;
    std::atomic<int64> nextSessionId_{0};

    // 会话ID -> 会话
    HashMap<int64, Ptr<Session>> sessions_;

    // 玩家ID -> 会话ID
    HashMap<int64, int64> playerSessions_;

    // 角色ID -> 会话ID
    HashMap<int64, int64> roleSessions_;
};

} // namespace legend