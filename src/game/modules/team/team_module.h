/**
 * @file team_module.h
 * @brief 组队模块
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"

namespace legend {

class Session;
class MySQLConnectionPool;

/**
 * @brief 队伍信息
 */
struct TeamInfo {
    int64 teamId;
    int64 leaderId;
    String teamName;
    Vector<int64> memberIds;
    int32 status;
};

/**
 * @brief 组队模块
 */
class TeamModule : public NonCopyable {
public:
    TeamModule();
    ~TeamModule();

    void init(MySQLConnectionPool* mysqlPool);

    // 创建队伍
    int64 createTeam(Session* session, const String& teamName);

    // 加入队伍
    bool joinTeam(Session* session, int64 teamId);

    // 离开队伍
    bool leaveTeam(Session* session);

    // 踢出成员
    bool kickMember(Session* session, int64 targetId);

    // 获取队伍信息
    Ptr<TeamInfo> getTeam(int64 teamId);

    // 获取玩家所在队伍
    Ptr<TeamInfo> getTeamByRole(int64 roleId);

private:
    MySQLConnectionPool* mysqlPool_;
};

} // namespace legend