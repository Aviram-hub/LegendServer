/**
 * @file team_manager.cpp
 * @brief 队伍管理器实现
 */

#include "team_manager.h"
#include "team_module.h"

namespace legend {

TeamManager& TeamManager::instance() {
    static TeamManager instance;
    return instance;
}

int64 TeamManager::createTeam(int64 leaderId) {
    MutexGuard guard(mutex_);

    int64 teamId = ++nextTeamId_;

    auto team = std::make_shared<TeamInfo>();
    team->teamId = teamId;
    team->leaderId = leaderId;
    team->memberIds.push_back(leaderId);
    team->status = 0;

    teams_[teamId] = team;
    roleTeams_[leaderId] = teamId;

    return teamId;
}

void TeamManager::dissolveTeam(int64 teamId) {
    MutexGuard guard(mutex_);

    auto it = teams_.find(teamId);
    if (it != teams_.end()) {
        for (int64 roleId : it->second->memberIds) {
            roleTeams_.erase(roleId);
        }
        teams_.erase(it);
    }
}

bool TeamManager::addMember(int64 teamId, int64 roleId) {
    MutexGuard guard(mutex_);

    auto it = teams_.find(teamId);
    if (it == teams_.end()) {
        return false;
    }

    it->second->memberIds.push_back(roleId);
    roleTeams_[roleId] = teamId;

    return true;
}

bool TeamManager::removeMember(int64 teamId, int64 roleId) {
    MutexGuard guard(mutex_);

    auto it = teams_.find(teamId);
    if (it == teams_.end()) {
        return false;
    }

    auto& members = it->second->memberIds;
    members.erase(std::remove(members.begin(), members.end(), roleId), members.end());
    roleTeams_.erase(roleId);

    return true;
}

Ptr<TeamInfo> TeamManager::getTeam(int64 teamId) {
    MutexGuard guard(mutex_);

    auto it = teams_.find(teamId);
    if (it != teams_.end()) {
        return it->second;
    }
    return nullptr;
}

int64 TeamManager::getTeamByRole(int64 roleId) {
    MutexGuard guard(mutex_);

    auto it = roleTeams_.find(roleId);
    if (it != roleTeams_.end()) {
        return it->second;
    }
    return 0;
}

} // namespace legend