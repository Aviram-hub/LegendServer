/**
 * @file battle_module.h
 * @brief 战斗模块
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"

namespace legend {

class Session;
class BattleScene;

/**
 * @brief 战斗模块
 */
class BattleModule : public NonCopyable {
public:
    BattleModule();
    ~BattleModule();

    void init();

    // 创建战斗
    int64 createBattle(const Vector<int64>& attackers, const Vector<int64>& defenders);

    // 结束战斗
    void endBattle(int64 battleId);

    // 获取战斗场景
    Ptr<BattleScene> getBattle(int64 battleId);

private:
    HashMap<int64, Ptr<BattleScene>> battles_;
    std::atomic<int64> nextBattleId_{0};
};

} // namespace legend