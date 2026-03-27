/**
 * @file entity_manager.h
 * @brief 实体管理器
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "entity.h"
#include "common/thread/mutex.h"
#include <unordered_map>

namespace legend {

class MySQLConnectionPool;

/**
 * @brief 实体管理器
 */
class EntityManager : public NonCopyable {
public:
    EntityManager();
    ~EntityManager();

    // 初始化
    bool init(MySQLConnectionPool* pool);

    // 获取玩家账号
    Ptr<PlayerAccountEntity> getPlayerAccount(int64 accountId);
    Ptr<PlayerAccountEntity> getPlayerAccountByName(const String& accountName);

    // 创建玩家账号
    Ptr<PlayerAccountEntity> createPlayerAccount(const String& accountName,
                                                  const String& password);

    // 保存玩家账号
    bool savePlayerAccount(Ptr<PlayerAccountEntity> entity);

    // 获取角色
    Ptr<RoleEntity> getRole(int64 roleId);
    Ptr<RoleEntity> getRoleByName(const String& roleName);
    Vector<Ptr<RoleEntity>> getRolesByAccountId(int64 accountId);

    // 创建角色
    Ptr<RoleEntity> createRole(int64 accountId, const String& roleName,
                                int profession, int gender);

    // 保存角色
    bool saveRole(Ptr<RoleEntity> entity);

    // 删除角色
    bool deleteRole(int64 roleId);

    // 缓存相关
    void cacheRole(Ptr<RoleEntity> role);
    void uncacheRole(int64 roleId);

private:
    MySQLConnectionPool* pool_;
    HashMap<int64, Ptr<RoleEntity>> roleCache_;
    HashMap<String, int64> roleNameIndex_;
    mutable Mutex mutex_;
};

} // namespace legend