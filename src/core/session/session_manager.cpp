/**
 * @file session_manager.cpp
 * @brief 会话管理器实现
 */

#include "session_manager.h"
#include "core/message/message.h"

namespace legend {

SessionManager& SessionManager::instance() {
    static SessionManager instance;
    return instance;
}

Ptr<Session> SessionManager::createSession() {
    MutexGuard guard(mutex_);
    int64 id = ++nextSessionId_;
    auto session = std::make_shared<Session>(id);
    sessions_[id] = session;
    return session;
}

Ptr<Session> SessionManager::getSession(int64 sessionId) {
    MutexGuard guard(mutex_);
    auto it = sessions_.find(sessionId);
    if (it != sessions_.end()) {
        return it->second;
    }
    return nullptr;
}

Ptr<Session> SessionManager::getSessionByPlayerId(int64 playerId) {
    MutexGuard guard(mutex_);
    auto it = playerSessions_.find(playerId);
    if (it != playerSessions_.end()) {
        auto sessionIt = sessions_.find(it->second);
        if (sessionIt != sessions_.end()) {
            return sessionIt->second;
        }
    }
    return nullptr;
}

Ptr<Session> SessionManager::getSessionByRoleId(int64 roleId) {
    MutexGuard guard(mutex_);
    auto it = roleSessions_.find(roleId);
    if (it != roleSessions_.end()) {
        auto sessionIt = sessions_.find(it->second);
        if (sessionIt != sessions_.end()) {
            return sessionIt->second;
        }
    }
    return nullptr;
}

void SessionManager::removeSession(int64 sessionId) {
    MutexGuard guard(mutex_);
    auto it = sessions_.find(sessionId);
    if (it != sessions_.end()) {
        auto& session = it->second;
        if (session->playerId() > 0) {
            playerSessions_.erase(session->playerId());
        }
        if (session->roleId() > 0) {
            roleSessions_.erase(session->roleId());
        }
        sessions_.erase(it);
    }
}

bool SessionManager::bindPlayer(int64 sessionId, int64 playerId) {
    MutexGuard guard(mutex_);
    auto it = sessions_.find(sessionId);
    if (it == sessions_.end()) {
        return false;
    }

    // 检查玩家是否已经绑定其他会话
    auto playerIt = playerSessions_.find(playerId);
    if (playerIt != playerSessions_.end() && playerIt->second != sessionId) {
        return false;
    }

    it->second->setPlayerId(playerId);
    playerSessions_[playerId] = sessionId;
    return true;
}

bool SessionManager::bindRole(int64 sessionId, int64 roleId) {
    MutexGuard guard(mutex_);
    auto it = sessions_.find(sessionId);
    if (it == sessions_.end()) {
        return false;
    }

    auto roleIt = roleSessions_.find(roleId);
    if (roleIt != roleSessions_.end() && roleIt->second != sessionId) {
        return false;
    }

    it->second->setRoleId(roleId);
    roleSessions_[roleId] = sessionId;
    return true;
}

void SessionManager::unbindPlayer(int64 playerId) {
    MutexGuard guard(mutex_);
    auto it = playerSessions_.find(playerId);
    if (it != playerSessions_.end()) {
        auto sessionIt = sessions_.find(it->second);
        if (sessionIt != sessions_.end()) {
            sessionIt->second->setPlayerId(0);
        }
        playerSessions_.erase(it);
    }
}

void SessionManager::unbindRole(int64 roleId) {
    MutexGuard guard(mutex_);
    auto it = roleSessions_.find(roleId);
    if (it != roleSessions_.end()) {
        auto sessionIt = sessions_.find(it->second);
        if (sessionIt != sessions_.end()) {
            sessionIt->second->setRoleId(0);
        }
        roleSessions_.erase(it);
    }
}

size_t SessionManager::sessionCount() const {
    MutexGuard guard(mutex_);
    return sessions_.size();
}

size_t SessionManager::onlinePlayerCount() const {
    MutexGuard guard(mutex_);
    return playerSessions_.size();
}

bool SessionManager::hasSession(int64 sessionId) const {
    MutexGuard guard(mutex_);
    return sessions_.find(sessionId) != sessions_.end();
}

bool SessionManager::isPlayerOnline(int64 playerId) const {
    MutexGuard guard(mutex_);
    return playerSessions_.find(playerId) != playerSessions_.end();
}

void SessionManager::broadcast(const Message& msg) {
    MutexGuard guard(mutex_);
    for (const auto& pair : sessions_) {
        if (pair.second->state() == SessionState::kPlaying) {
            pair.second->send(msg);
        }
    }
}

} // namespace legend