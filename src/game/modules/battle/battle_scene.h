/**
 * @file battle_scene.h
 * @brief 战斗场景
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"

namespace legend {

/**
 * @brief 战斗单位
 */
struct BattleUnit {
    int64 roleId;
    String name;
    int32 hp;
    int32 hpMax;
    int32 mp;
    int32 mpMax;
    int32 attack;
    int32 defense;
    int32 speed;
    float criticalRate;
    float criticalDamage;
    bool isAlive;
};

/**
 * @brief 战斗场景
 */
class BattleScene : public NonCopyable {
public:
    BattleScene(int64 id);
    ~BattleScene();

    // 添加参战单位
    void addAttacker(const BattleUnit& unit);
    void addDefender(const BattleUnit& unit);

    // 执行回合
    void executeRound();

    // 检查战斗是否结束
    int checkEnd();  // 0=进行中, 1=攻击方胜, 2=防御方胜, 3=平局

    // 获取战斗信息
    int64 id() const { return id_; }
    int round() const { return round_; }
    const Vector<BattleUnit>& attackers() const { return attackers_; }
    const Vector<BattleUnit>& defenders() const { return defenders_; }

private:
    int64 id_;
    int round_;
    int maxRounds_;

    Vector<BattleUnit> attackers_;
    Vector<BattleUnit> defenders_;
};

} // namespace legend