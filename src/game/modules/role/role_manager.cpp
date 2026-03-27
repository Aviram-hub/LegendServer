/**
 * @file role_manager.cpp
 * @brief 角色管理器实现
 */

#include "role_manager.h"
#include "database/orm/entity.h"

namespace legend {

RoleManager& RoleManager::instance() {
    static RoleManager instance;
    return instance;
}

void RoleManager::cacheRole(Ptr<RoleEntity> role) {
    if (!role) return;
    MutexGuard guard(mutex_);
    roleCache_[role->id()] = role;
}

void RoleManager::uncacheRole(int64 roleId) {
    MutexGuard guard(mutex_);
    roleCache_.erase(roleId);
}

Ptr<RoleEntity> RoleManager::getRole(int64 roleId) {
    MutexGuard guard(mutex_);
    auto it = roleCache_.find(roleId);
    if (it != roleCache_.end()) {
        return it->second;
    }
    return nullptr;
}

size_t RoleManager::cacheSize() const {
    MutexGuard guard(mutex_);
    return roleCache_.size();
}

} // namespace legend