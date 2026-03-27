/**
 * @file entity_manager.cpp
 * @brief 实体管理器实现
 */

#include "entity_manager.h"
#include "database/mysql/mysql_connection_pool.h"

namespace legend {

EntityManager::EntityManager()
    : pool_(nullptr) {
}

EntityManager::~EntityManager() = default;

bool EntityManager::init(MySQLConnectionPool* pool) {
    pool_ = pool;
    return pool_ != nullptr;
}

Ptr<PlayerAccountEntity> EntityManager::getPlayerAccount(int64 accountId) {
    MySQLConnectionGuard conn(*pool_);
    if (!conn) return nullptr;

    MySQLResult result = conn->executeQuery(
        "SELECT * FROM player_account WHERE account_id = ?",
        {MySQLParam(accountId)});

    if (result.next()) {
        auto entity = std::make_shared<PlayerAccountEntity>();
        entity->setId(result.getInt64("account_id"));
        entity->setAccountName(result.getString("account_name"));
        entity->setPasswordHash(result.getString("password_hash"));
        entity->setSalt(result.getString("salt"));
        entity->setRegisterTime(result.getInt64("register_time"));
        entity->setLastLoginTime(result.getInt64("last_login_time"));
        entity->setLoginIp(result.getString("login_ip"));
        entity->setStatus(result.getInt32("status"));
        entity->clearDirty();
        return entity;
    }

    return nullptr;
}

Ptr<PlayerAccountEntity> EntityManager::getPlayerAccountByName(const String& accountName) {
    MySQLConnectionGuard conn(*pool_);
    if (!conn) return nullptr;

    MySQLResult result = conn->executeQuery(
        "SELECT * FROM player_account WHERE account_name = ?",
        {MySQLParam(accountName)});

    if (result.next()) {
        auto entity = std::make_shared<PlayerAccountEntity>();
        entity->setId(result.getInt64("account_id"));
        entity->setAccountName(result.getString("account_name"));
        entity->setPasswordHash(result.getString("password_hash"));
        entity->setSalt(result.getString("salt"));
        entity->setRegisterTime(result.getInt64("register_time"));
        entity->setLastLoginTime(result.getInt64("last_login_time"));
        entity->setLoginIp(result.getString("login_ip"));
        entity->setStatus(result.getInt32("status"));
        entity->clearDirty();
        return entity;
    }

    return nullptr;
}

Ptr<PlayerAccountEntity> EntityManager::createPlayerAccount(const String& accountName,
                                                             const String& password) {
    auto entity = std::make_shared<PlayerAccountEntity>();
    entity->setAccountName(accountName);
    entity->setPasswordHash(password);  // 应该先加密
    entity->setSalt("random_salt");     // 应该生成随机盐
    entity->setRegisterTime(nowMs());

    MySQLConnectionGuard conn(*pool_);
    if (conn && entity->save(*conn)) {
        return entity;
    }

    return nullptr;
}

bool EntityManager::savePlayerAccount(Ptr<PlayerAccountEntity> entity) {
    MySQLConnectionGuard conn(*pool_);
    return conn && entity->save(*conn);
}

Ptr<RoleEntity> EntityManager::getRole(int64 roleId) {
    {
        MutexGuard guard(mutex_);
        auto it = roleCache_.find(roleId);
        if (it != roleCache_.end()) {
            return it->second;
        }
    }

    MySQLConnectionGuard conn(*pool_);
    if (!conn) return nullptr;

    MySQLResult result = conn->executeQuery(
        "SELECT * FROM player_role WHERE role_id = ?",
        {MySQLParam(roleId)});

    if (result.next()) {
        auto entity = std::make_shared<RoleEntity>();
        entity->setId(result.getInt64("role_id"));
        entity->setAccountId(result.getInt64("account_id"));
        entity->setRoleName(result.getString("role_name"));
        entity->setServerId(result.getInt32("server_id"));
        entity->setLevel(result.getInt32("level"));
        entity->setExp(result.getInt64("exp"));
        entity->setProfession(result.getInt32("profession"));
        entity->setGender(result.getInt32("gender"));
        entity->setHp(result.getInt32("hp"));
        entity->setMp(result.getInt32("mp"));
        entity->setGold(result.getInt64("gold"));
        entity->setDiamond(result.getInt32("diamond"));
        entity->clearDirty();

        cacheRole(entity);
        return entity;
    }

    return nullptr;
}

Ptr<RoleEntity> EntityManager::getRoleByName(const String& roleName) {
    MySQLConnectionGuard conn(*pool_);
    if (!conn) return nullptr;

    MySQLResult result = conn->executeQuery(
        "SELECT role_id FROM player_role WHERE role_name = ?",
        {MySQLParam(roleName)});

    if (result.next()) {
        return getRole(result.getInt64(0));
    }

    return nullptr;
}

Vector<Ptr<RoleEntity>> EntityManager::getRolesByAccountId(int64 accountId) {
    Vector<Ptr<RoleEntity>> roles;

    MySQLConnectionGuard conn(*pool_);
    if (!conn) return roles;

    MySQLResult result = conn->executeQuery(
        "SELECT role_id FROM player_role WHERE account_id = ?",
        {MySQLParam(accountId)});

    while (result.next()) {
        auto role = getRole(result.getInt64(0));
        if (role) {
            roles.push_back(role);
        }
    }

    return roles;
}

Ptr<RoleEntity> EntityManager::createRole(int64 accountId, const String& roleName,
                                           int profession, int gender) {
    auto entity = std::make_shared<RoleEntity>();
    entity->setAccountId(accountId);
    entity->setRoleName(roleName);
    entity->setProfession(profession);
    entity->setGender(gender);

    MySQLConnectionGuard conn(*pool_);
    if (conn && entity->save(*conn)) {
        cacheRole(entity);
        return entity;
    }

    return nullptr;
}

bool EntityManager::saveRole(Ptr<RoleEntity> entity) {
    MySQLConnectionGuard conn(*pool_);
    if (conn && entity->save(*conn)) {
        cacheRole(entity);
        return true;
    }
    return false;
}

bool EntityManager::deleteRole(int64 roleId) {
    MySQLConnectionGuard conn(*pool_);
    if (!conn) return false;

    int64 affected = conn->executeUpdate(
        "DELETE FROM player_role WHERE role_id = ?",
        {MySQLParam(roleId)});

    if (affected > 0) {
        uncacheRole(roleId);
        return true;
    }
    return false;
}

void EntityManager::cacheRole(Ptr<RoleEntity> role) {
    MutexGuard guard(mutex_);
    roleCache_[role->id()] = role;
    roleNameIndex_[role->roleName()] = role->id();
}

void EntityManager::uncacheRole(int64 roleId) {
    MutexGuard guard(mutex_);
    auto it = roleCache_.find(roleId);
    if (it != roleCache_.end()) {
        roleNameIndex_.erase(it->second->roleName());
        roleCache_.erase(it);
    }
}

} // namespace legend