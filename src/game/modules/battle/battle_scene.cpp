/**
 * @file battle_scene.cpp
 * @brief 战斗场景实现
 */

#include "battle_scene.h"

namespace legend {

BattleScene::BattleScene(int64 id)
    : id_(id)
    , round_(0)
    , maxRounds_(30) {
}

BattleScene::~BattleScene() = default;

void BattleScene::addAttacker(const BattleUnit& unit) {
    attackers_.push_back(unit);
}

void BattleScene::addDefender(const BattleUnit& unit) {
    defenders_.push_back(unit);
}

void BattleScene::executeRound() {
    ++round_;

    // 执行回合战斗逻辑
    // 简化实现
}

int BattleScene::checkEnd() {
    bool attackersAlive = false;
    bool defendersAlive = false;

    for (const auto& unit : attackers_) {
        if (unit.isAlive) attackersAlive = true;
    }

    for (const auto& unit : defenders_) {
        if (unit.isAlive) defendersAlive = true;
    }

    if (!attackersAlive) return 2;  // 防御方胜
    if (!defendersAlive) return 1;  // 攻击方胜
    if (round_ >= maxRounds_) return 3;  // 平局

    return 0;  // 进行中
}

} // namespace legend