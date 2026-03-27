/**
 * @file battle_manager.h
 * @brief 战斗管理器
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "common/thread/mutex.h"

namespace legend {

class BattleScene;

/**
 * @brief 战斗管理器
 */
class BattleManager : public NonCopyable {
public:
    static BattleManager& instance();

    int64 createBattle();
    void removeBattle(int64 battleId);
    Ptr<BattleScene> getBattle(int64 battleId);

private:
    BattleManager() = default;

    mutable Mutex mutex_;
    HashMap<int64, Ptr<BattleScene>> battles_;
    std::atomic<int64> nextBattleId_{0};
};

} // namespace legend