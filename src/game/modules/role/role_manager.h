/**
 * @file role_manager.h
 * @brief 角色管理器
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "common/thread/mutex.h"
#include <unordered_map>

namespace legend {

class RoleEntity;

/**
 * @brief 角色管理器
 */
class RoleManager : public NonCopyable {
public:
    static RoleManager& instance();

    void cacheRole(Ptr<RoleEntity> role);
    void uncacheRole(int64 roleId);
    Ptr<RoleEntity> getRole(int64 roleId);

    size_t cacheSize() const;

private:
    RoleManager() = default;

    mutable Mutex mutex_;
    HashMap<int64, Ptr<RoleEntity>> roleCache_;
};

} // namespace legend