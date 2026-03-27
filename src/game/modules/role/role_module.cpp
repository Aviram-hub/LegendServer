/**
 * @file role_module.cpp
 * @brief 角色模块实现
 */

#include "role_module.h"
#include "role_manager.h"
#include "core/session/session.h"
#include "database/mysql/mysql_connection_pool.h"
#include "database/orm/entity.h"

namespace legend {

RoleModule::RoleModule()
    : mysqlPool_(nullptr) {
}

RoleModule::~RoleModule() = default;

void RoleModule::init(MySQLConnectionPool* mysqlPool) {
    mysqlPool_ = mysqlPool;
}

int64 RoleModule::createRole(Session* session, const String& name, int profession, int gender) {
    if (!session || !mysqlPool_) {
        return 0;
    }

    MySQLConnectionGuard conn(*mysqlPool_);
    if (!conn) {
        return 0;
    }

    // 检查角色名是否存在
    MySQLResult result = conn->executeQuery(
        "SELECT role_id FROM player_role WHERE role_name = ?",
        {MySQLParam(name)});

    if (result.next()) {
        return 0;  // 角色名已存在
    }

    // 创建角色
    int64 roleId = conn->insertAndGetId(
        "INSERT INTO player_role (account_id, role_name, profession, gender, hp, mp) VALUES (?, ?, ?, ?, 100, 100)",
        {MySQLParam(session->playerId()), MySQLParam(name),
         MySQLParam(static_cast<int64>(profession)), MySQLParam(static_cast<int64>(gender))});

    if (roleId > 0) {
        // 创建角色属性
        conn->executeUpdate(
            "INSERT INTO role_attribute (role_id, attack, defense, speed, hp_max, mp_max) VALUES (?, 10, 10, 10, 100, 100)",
            {MySQLParam(roleId)});
    }

    return roleId;
}

Ptr<RoleEntity> RoleModule::getRole(int64 roleId) {
    // 先从缓存查找
    auto cachedRole = RoleManager::instance().getRole(roleId);
    if (cachedRole) {
        return cachedRole;
    }

    // 从数据库加载
    if (!mysqlPool_) {
        return nullptr;
    }

    MySQLConnectionGuard conn(*mysqlPool_);
    if (!conn) {
        return nullptr;
    }

    MySQLResult result = conn->executeQuery(
        "SELECT * FROM player_role WHERE role_id = ?",
        {MySQLParam(roleId)});

    if (!result.next()) {
        return nullptr;
    }

    auto role = std::make_shared<RoleEntity>();
    role->setId(result.getInt64("role_id"));
    role->setAccountId(result.getInt64("account_id"));
    role->setRoleName(result.getString("role_name"));
    role->setLevel(result.getInt32("level"));
    role->setExp(result.getInt64("exp"));
    role->setProfession(result.getInt32("profession"));
    role->setGender(result.getInt32("gender"));
    role->setHp(result.getInt32("hp"));
    role->setMp(result.getInt32("mp"));
    role->setGold(result.getInt64("gold"));
    role->setDiamond(result.getInt32("diamond"));
    role->clearDirty();

    // 缓存角色
    RoleManager::instance().cacheRole(role);

    return role;
}

Vector<int64> RoleModule::getRoleIdsByAccountId(int64 accountId) {
    Vector<int64> roleIds;

    if (!mysqlPool_) {
        return roleIds;
    }

    MySQLConnectionGuard conn(*mysqlPool_);
    if (!conn) {
        return roleIds;
    }

    MySQLResult result = conn->executeQuery(
        "SELECT role_id FROM player_role WHERE account_id = ?",
        {MySQLParam(accountId)});

    while (result.next()) {
        roleIds.push_back(result.getInt64(0));
    }

    return roleIds;
}

bool RoleModule::selectRole(Session* session, int64 roleId) {
    if (!session) {
        return false;
    }

    auto role = getRole(roleId);
    if (!role) {
        return false;
    }

    if (role->accountId() != session->playerId()) {
        return false;
    }

    session->setRoleId(roleId);
    return true;
}

void RoleModule::updateRole(Ptr<RoleEntity> role) {
    if (!role || !mysqlPool_) {
        return;
    }

    MySQLConnectionGuard conn(*mysqlPool_);
    if (!conn) {
        return;
    }

    role->save(*conn);
}

} // namespace legend