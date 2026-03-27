/**
 * @file skill_system.h
 * @brief 技能系统
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"

namespace legend {

/**
 * @brief 技能配置
 */
struct SkillConfig {
    int32 id{0};
    String name;
    int32 type{0};       // 1=攻击, 2=治疗, 3=增益, 4=减益
    int32 mpCost{0};
    int32 cooldown{0};
    float damageRatio{1.0f};
    int32 range{1};
    bool aoe{false};
};

/**
 * @brief 技能系统
 */
class SkillSystem : public NonCopyable {
public:
    SkillSystem();
    ~SkillSystem();

    void init();

    // 获取技能配置
    const SkillConfig* getSkill(int32 skillId) const;

    // 计算伤害
    int32 calculateDamage(const SkillConfig& skill, int32 attack, int32 defense);

    // 计算治疗
    int32 calculateHeal(const SkillConfig& skill, int32 magic);

private:
    HashMap<int32, SkillConfig> skills_;
};

} // namespace legend