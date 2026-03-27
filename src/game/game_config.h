/**
 * @file game_config.h
 * @brief 游戏配置
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"

namespace legend {

/**
 * @brief 游戏配置类
 */
class GameConfig : public NonCopyable {
public:
    // 角色配置
    static constexpr int MAX_NAME_LENGTH = 16;
    static constexpr int MAX_ROLE_PER_ACCOUNT = 3;
    static constexpr int DEFAULT_HP = 100;
    static constexpr int DEFAULT_MP = 100;

    // 战斗配置
    static constexpr int MAX_BATTLE_ROUNDS = 30;
    static constexpr int SKILL_CD_BASE = 1000;

    // 聊天配置
    static constexpr int CHAT_MAX_LENGTH = 256;
    static constexpr int WORLD_CHAT_CD = 5000;
    static constexpr int PRIVATE_CHAT_CD = 1000;

    // 好友配置
    static constexpr int MAX_FRIEND_COUNT = 100;

    // 组队配置
    static constexpr int MAX_TEAM_MEMBERS = 5;
};

} // namespace legend