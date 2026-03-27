/**
 * @file battle_manager.cpp
 * @brief 战斗管理器实现
 */

#include "battle_manager.h"
#include "battle_scene.h"

namespace legend {

BattleManager& BattleManager::instance() {
    static BattleManager instance;
    return instance;
}

int64 BattleManager::createBattle() {
    MutexGuard guard(mutex_);
    int64 id = ++nextBattleId_;
    battles_[id] = std::make_shared<BattleScene>(id);
    return id;
}

void BattleManager::removeBattle(int64 battleId) {
    MutexGuard guard(mutex_);
    battles_.erase(battleId);
}

Ptr<BattleScene> BattleManager::getBattle(int64 battleId) {
    MutexGuard guard(mutex_);
    auto it = battles_.find(battleId);
    if (it != battles_.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace legend