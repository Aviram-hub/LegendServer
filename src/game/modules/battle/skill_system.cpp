/**
 * @file skill_system.cpp
 * @brief 技能系统实现
 */

#include "skill_system.h"

namespace legend {

SkillSystem::SkillSystem() {
    init();
}

SkillSystem::~SkillSystem() = default;

void SkillSystem::init() {
    // 初始化技能配置
    SkillConfig skill;

    // 战士技能
    skill = {1001, "普通攻击", 1, 0, 0, 1.0f, 1, false};
    skills_[1001] = skill;

    skill = {1002, "重击", 1, 10, 2000, 1.5f, 1, false};
    skills_[1002] = skill;

    skill = {1003, "旋风斩", 1, 20, 5000, 0.8f, 2, true};
    skills_[1003] = skill;

    // 法师技能
    skill = {2001, "火球术", 1, 15, 1500, 1.2f, 5, false};
    skills_[2001] = skill;

    skill = {2002, "冰霜新星", 1, 25, 4000, 1.0f, 4, true};
    skills_[2002] = skill;

    // 弓手技能
    skill = {3001, "穿刺射击", 1, 12, 1000, 1.1f, 6, false};
    skills_[3001] = skill;

    skill = {3002, "多重射击", 1, 20, 3000, 0.6f, 5, false};
    skills_[3002] = skill;
}

const SkillConfig* SkillSystem::getSkill(int32 skillId) const {
    auto it = skills_.find(skillId);
    if (it != skills_.end()) {
        return &it->second;
    }
    return nullptr;
}

int32 SkillSystem::calculateDamage(const SkillConfig& skill, int32 attack, int32 defense) {
    float baseDamage = attack * skill.damageRatio;
    float reduction = static_cast<float>(defense) / (defense + 100.0f);
    return static_cast<int32>(baseDamage * (1.0f - reduction));
}

int32 SkillSystem::calculateHeal(const SkillConfig& skill, int32 magic) {
    return static_cast<int32>(magic * 0.5f);
}

} // namespace legend