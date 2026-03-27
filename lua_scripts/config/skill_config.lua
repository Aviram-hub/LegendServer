-- 技能配置

local M = {
    -- 技能类型
    SKILL_TYPE = {
        ATTACK = 1,     -- 攻击
        HEAL = 2,       -- 治疗
        BUFF = 3,       -- 增益
        DEBUFF = 4,     -- 减益
    },

    -- 技能配置
    skills = {
        -- 战士技能
        [1001] = {
            id = 1001,
            name = "普通攻击",
            type = 1,
            mp_cost = 0,
            cooldown = 0,
            damage_ratio = 1.0,
            range = 1,
            description = "普通物理攻击",
        },
        [1002] = {
            id = 1002,
            name = "重击",
            type = 1,
            mp_cost = 10,
            cooldown = 2000,
            damage_ratio = 1.5,
            range = 1,
            description = "造成150%伤害",
        },
        [1003] = {
            id = 1003,
            name = "旋风斩",
            type = 1,
            mp_cost = 20,
            cooldown = 5000,
            damage_ratio = 0.8,
            range = 2,
            aoe = true,
            description = "对周围敌人造成伤害",
        },

        -- 法师技能
        [2001] = {
            id = 2001,
            name = "火球术",
            type = 1,
            mp_cost = 15,
            cooldown = 1500,
            damage_ratio = 1.2,
            range = 5,
            element = "fire",
            description = "发射火球造成伤害",
        },
        [2002] = {
            id = 2002,
            name = "冰霜新星",
            type = 1,
            mp_cost = 25,
            cooldown = 4000,
            damage_ratio = 1.0,
            range = 4,
            aoe = true,
            element = "ice",
            slow = 0.5,
            description = "冰冻周围敌人",
        },
        [2003] = {
            id = 2003,
            name = "治愈术",
            type = 2,
            mp_cost = 30,
            cooldown = 6000,
            heal_ratio = 0.3,
            range = 3,
            description = "恢复目标生命值",
        },

        -- 弓手技能
        [3001] = {
            id = 3001,
            name = "穿刺射击",
            type = 1,
            mp_cost = 12,
            cooldown = 1000,
            damage_ratio = 1.1,
            range = 6,
            description = "精准射击",
        },
        [3002] = {
            id = 3002,
            name = "多重射击",
            type = 1,
            mp_cost = 20,
            cooldown = 3000,
            damage_ratio = 0.6,
            range = 5,
            multi_target = 3,
            description = "同时攻击多个目标",
        },
        [3003] = {
            id = 3003,
            name = "致命一击",
            type = 1,
            mp_cost = 35,
            cooldown = 8000,
            damage_ratio = 2.5,
            range = 5,
            crit_bonus = 0.5,
            description = "造成致命伤害",
        },
    },
}

-- 获取技能配置
function M.get(skill_id)
    return M.skills[skill_id]
end

-- 检查技能是否存在
function M.exists(skill_id)
    return M.skills[skill_id] ~= nil
end

return M