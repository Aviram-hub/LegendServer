/**
 * @file team_manager.h
 * @brief 队伍管理器
 */

#pragma once

#include "types.h"
#include "noncopyable.h"
#include "common/thread/mutex.h"

namespace legend {

class TeamInfo;

/**
 * @brief 队伍管理器
 */
class TeamManager : public NonCopyable {
public:
    static TeamManager& instance();

    // 创建队伍
    int64 createTeam(int64 leaderId);

    // 解散队伍
    void dissolveTeam(int64 teamId);

    // 添加成员
    bool addMember(int64 teamId, int64 roleId);

    // 移除成员
    bool removeMember(int64 teamId, int64 roleId);

    // 获取队伍
    Ptr<TeamInfo> getTeam(int64 teamId);

    // 获取玩家所在队伍
    int64 getTeamByRole(int64 roleId);

private:
    TeamManager() = default;

    mutable Mutex mutex_;
    HashMap<int64, Ptr<TeamInfo>> teams_;
    HashMap<int64, int64> roleTeams_;  // roleId -> teamId
    std::atomic<int64> nextTeamId_{0};
};

} // namespace legend