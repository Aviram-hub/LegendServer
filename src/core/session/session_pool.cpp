/**
 * @file session_pool.cpp
 * @brief 会话池实现
 */

#include "session_pool.h"

namespace legend {

SessionPool::SessionPool(size_t initialSize)
    : capacity_(initialSize) {
    preallocate(initialSize);
}

Ptr<Session> SessionPool::acquire() {
    MutexGuard guard(mutex_);

    if (pool_.empty()) {
        preallocate(capacity_);
    }

    if (pool_.empty()) {
        int64 id = ++nextId_;
        return std::make_shared<Session>(id);
    }

    auto session = pool_.back();
    pool_.pop_back();
    return session;
}

void SessionPool::release(Ptr<Session> session) {
    if (!session) return;

    session->setPlayerId(0);
    session->setRoleId(0);
    session->setState(SessionState::kNone);
    session->setToken("");
    session->setContext(nullptr);

    MutexGuard guard(mutex_);
    pool_.push_back(session);
}

void SessionPool::preallocate(size_t count) {
    for (size_t i = 0; i < count; ++i) {
        int64 id = ++nextId_;
        pool_.push_back(std::make_shared<Session>(id));
    }
}

} // namespace legend