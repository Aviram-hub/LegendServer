/**
 * @file team_module.cpp
 * @brief 组队模块实现
 */

#include "team_module.h"
#include "team_manager.h"
#include "core/session/session.h"
#include "database/mysql/mysql_connection_pool.h"

namespace legend {

TeamModule::TeamModule()
    : mysqlPool_(nullptr) {
}

TeamModule::~TeamModule() = default;

void TeamModule::init(MySQLConnectionPool* mysqlPool) {
    mysqlPool_ = mysqlPool;
}

int64 TeamModule::createTeam(Session* session, const String& teamName) {
    if (!session || !mysqlPool_) {
        return 0;
    }

    // 检查是否已在队伍中
    auto existingTeam = getTeamByRole(session->roleId());
    if (existingTeam) {
        return 0;
    }

    MySQLConnectionGuard conn(*mysqlPool_);
    if (!conn) {
        return 0;
    }

    // 创建队伍
    int64 teamId = conn->insertAndGetId(
        "INSERT INTO team (leader_role_id, team_name) VALUES (?, ?)",
        {MySQLParam(session->roleId()), MySQLParam(teamName)});

    if (teamId > 0) {
        // 添加队长为成员
        conn->executeUpdate(
            "INSERT INTO team_member (team_id, role_id) VALUES (?, ?)",
            {MySQLParam(teamId), MySQLParam(session->roleId())});
    }

    return teamId;
}

bool TeamModule::joinTeam(Session* session, int64 teamId) {
    if (!session || !mysqlPool_) {
        return false;
    }

    // 检查是否已在队伍中
    auto existingTeam = getTeamByRole(session->roleId());
    if (existingTeam) {
        return false;
    }

    MySQLConnectionGuard conn(*mysqlPool_);
    if (!conn) {
        return false;
    }

    // 检查队伍是否存在且未满
    MySQLResult result = conn->executeQuery(
        "SELECT t.team_id, COUNT(m.role_id) as member_count "
        "FROM team t LEFT JOIN team_member m ON t.team_id = m.team_id "
        "WHERE t.team_id = ? AND t.status = 0 GROUP BY t.team_id",
        {MySQLParam(teamId)});

    if (!result.next()) {
        return false;
    }

    int32 memberCount = result.getInt32("member_count");
    if (memberCount >= 5) {  // 最大成员数
        return false;
    }

    // 加入队伍
    conn->executeUpdate(
        "INSERT INTO team_member (team_id, role_id) VALUES (?, ?)",
        {MySQLParam(teamId), MySQLParam(session->roleId())});

    return true;
}

bool TeamModule::leaveTeam(Session* session) {
    if (!session || !mysqlPool_) {
        return false;
    }

    auto team = getTeamByRole(session->roleId());
    if (!team) {
        return false;
    }

    MySQLConnectionGuard conn(*mysqlPool_);
    if (!conn) {
        return false;
    }

    // 移除成员
    conn->executeUpdate(
        "DELETE FROM team_member WHERE team_id = ? AND role_id = ?",
        {MySQLParam(team->teamId), MySQLParam(session->roleId())});

    // 如果是队长离开，转移队长或解散队伍
    if (team->leaderId == session->roleId()) {
        MySQLResult result = conn->executeQuery(
            "SELECT role_id FROM team_member WHERE team_id = ? ORDER BY join_time ASC LIMIT 1",
            {MySQLParam(team->teamId)});

        if (result.next()) {
            // 转移队长
            int64 newLeader = result.getInt64(0);
            conn->executeUpdate(
                "UPDATE team SET leader_role_id = ? WHERE team_id = ?",
                {MySQLParam(newLeader), MySQLParam(team->teamId)});
        } else {
            // 解散队伍
            conn->executeUpdate(
                "UPDATE team SET status = 1 WHERE team_id = ?",
                {MySQLParam(team->teamId)});
        }
    }

    return true;
}

bool TeamModule::kickMember(Session* session, int64 targetId) {
    if (!session || !mysqlPool_) {
        return false;
    }

    auto team = getTeamByRole(session->roleId());
    if (!team || team->leaderId != session->roleId()) {
        return false;
    }

    MySQLConnectionGuard conn(*mysqlPool_);
    if (!conn) {
        return false;
    }

    conn->executeUpdate(
        "DELETE FROM team_member WHERE team_id = ? AND role_id = ?",
        {MySQLParam(team->teamId), MySQLParam(targetId)});

    return true;
}

Ptr<TeamInfo> TeamModule::getTeam(int64 teamId) {
    return TeamManager::instance().getTeam(teamId);
}

Ptr<TeamInfo> TeamModule::getTeamByRole(int64 roleId) {
    int64 teamId = TeamManager::instance().getTeamByRole(roleId);
    if (teamId > 0) {
        return getTeam(teamId);
    }
    return nullptr;
}

} // namespace legend