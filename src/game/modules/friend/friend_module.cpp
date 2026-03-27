/**
 * @file friend_module.cpp
 * @brief 好友模块实现
 */

#include "friend_module.h"
#include "friend_manager.h"
#include "core/session/session.h"
#include "database/mysql/mysql_connection_pool.h"

namespace legend {

FriendModule::FriendModule()
    : mysqlPool_(nullptr) {
}

FriendModule::~FriendModule() = default;

void FriendModule::init(MySQLConnectionPool* mysqlPool) {
    mysqlPool_ = mysqlPool;
}

bool FriendModule::addFriend(Session* session, int64 friendRoleId) {
    if (!session || !mysqlPool_) {
        return false;
    }

    int64 roleId = session->roleId();

    // 检查是否已经是好友
    if (isFriend(roleId, friendRoleId)) {
        return false;
    }

    MySQLConnectionGuard conn(*mysqlPool_);
    if (!conn) {
        return false;
    }

    // 添加双向好友关系
    conn->executeUpdate(
        "INSERT INTO friend_relation (role_id, friend_role_id) VALUES (?, ?), (?, ?)",
        {MySQLParam(roleId), MySQLParam(friendRoleId),
         MySQLParam(friendRoleId), MySQLParam(roleId)});

    // 更新缓存
    FriendManager::instance().addFriend(roleId, friendRoleId);

    return true;
}

bool FriendModule::removeFriend(Session* session, int64 friendRoleId) {
    if (!session || !mysqlPool_) {
        return false;
    }

    int64 roleId = session->roleId();

    MySQLConnectionGuard conn(*mysqlPool_);
    if (!conn) {
        return false;
    }

    // 删除双向好友关系
    conn->executeUpdate(
        "DELETE FROM friend_relation WHERE (role_id = ? AND friend_role_id = ?) OR (role_id = ? AND friend_role_id = ?)",
        {MySQLParam(roleId), MySQLParam(friendRoleId),
         MySQLParam(friendRoleId), MySQLParam(roleId)});

    // 更新缓存
    FriendManager::instance().removeFriend(roleId, friendRoleId);

    return true;
}

Vector<FriendInfo> FriendModule::getFriendList(int64 roleId) {
    Vector<FriendInfo> friends;

    if (!mysqlPool_) {
        return friends;
    }

    MySQLConnectionGuard conn(*mysqlPool_);
    if (!conn) {
        return friends;
    }

    MySQLResult result = conn->executeQuery(
        "SELECT f.friend_role_id, f.intimacy, r.role_name, r.level, r.profession "
        "FROM friend_relation f "
        "LEFT JOIN player_role r ON f.friend_role_id = r.role_id "
        "WHERE f.role_id = ?",
        {MySQLParam(roleId)});

    while (result.next()) {
        FriendInfo info;
        info.roleId = result.getInt64("friend_role_id");
        info.name = result.getString("role_name");
        info.level = result.getInt32("level");
        info.profession = result.getInt32("profession");
        info.intimacy = result.getInt32("intimacy");
        info.online = false;  // 需要从Redis获取在线状态
        friends.push_back(info);
    }

    return friends;
}

bool FriendModule::isFriend(int64 roleId, int64 friendRoleId) {
    return FriendManager::instance().isFriend(roleId, friendRoleId);
}

} // namespace legend