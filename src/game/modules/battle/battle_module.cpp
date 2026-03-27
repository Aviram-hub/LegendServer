/**
 * @file battle_module.cpp
 * @brief 战斗模块实现
 */

#include "battle_module.h"
#include "battle_scene.h"

namespace legend {

BattleModule::BattleModule() = default;
BattleModule::~BattleModule() = default;

void BattleModule::init() {
}

int64 BattleModule::createBattle(const Vector<int64>& attackers, const Vector<int64>& defenders) {
    int64 battleId = ++nextBattleId_;
    auto scene = std::make_shared<BattleScene>(battleId);

    // 添加参战单位
    for (int64 roleId : attackers) {
        BattleUnit unit;
        unit.roleId = roleId;
        unit.hp = 100;
        unit.hpMax = 100;
        unit.isAlive = true;
        scene->addAttacker(unit);
    }

    for (int64 roleId : defenders) {
        BattleUnit unit;
        unit.roleId = roleId;
        unit.hp = 100;
        unit.hpMax = 100;
        unit.isAlive = true;
        scene->addDefender(unit);
    }

    battles_[battleId] = scene;
    return battleId;
}

void BattleModule::endBattle(int64 battleId) {
    battles_.erase(battleId);
}

Ptr<BattleScene> BattleModule::getBattle(int64 battleId) {
    auto it = battles_.find(battleId);
    if (it != battles_.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace legend