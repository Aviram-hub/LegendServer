/**
 * @file friend_module.h
 * @brief 好友模块
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"

namespace legend {

class Session;
class MySQLConnectionPool;

/**
 * @brief 好友信息
 */
struct FriendInfo {
    int64 roleId;
    String name;
    int32 level;
    int32 profession;
    int32 intimacy;
    bool online;
};

/**
 * @brief 好友模块
 */
class FriendModule : public NonCopyable {
public:
    FriendModule();
    ~FriendModule();

    void init(MySQLConnectionPool* mysqlPool);

    // 添加好友
    bool addFriend(Session* session, int64 friendRoleId);

    // 删除好友
    bool removeFriend(Session* session, int64 friendRoleId);

    // 获取好友列表
    Vector<FriendInfo> getFriendList(int64 roleId);

    // 检查好友关系
    bool isFriend(int64 roleId, int64 friendRoleId);

private:
    MySQLConnectionPool* mysqlPool_;
};

} // namespace legend