-- 战斗模块

local log = _G.log
local utils = _G.utils
local config = _G.config

local M = {}

-- 战斗状态
local battles = {}

-- 初始化
function M.init()
    log.info("Battle module initialized")
end

-- 创建战斗
function M.create_battle(battle_type, attackers, defenders)
    local battle_id = utils.now_ms()

    local battle = {
        id = battle_id,
        type = battle_type,
        round = 0,
        max_rounds = config.game.battle.max_rounds,
        attackers = {},
        defenders = {},
        start_time = utils.now(),
    }

    -- 初始化参战单位
    for i, role_id in ipairs(attackers) do
        battle.attackers[i] = M.create_unit(role_id)
    end

    for i, role_id in ipairs(defenders) do
        battle.defenders[i] = M.create_unit(role_id)
    end

    battles[battle_id] = battle

    log.info("Battle created: id=" .. battle_id)

    return battle
end

-- 创建战斗单位
function M.create_unit(role_id)
    local role = db.query([[
        SELECT r.role_id, r.role_name, r.hp, r.mp, r.level,
               a.attack, a.defense, a.speed, a.hp_max, a.mp_max,
               a.critical_rate, a.critical_damage
        FROM player_role r
        LEFT JOIN role_attribute a ON r.role_id = a.role_id
        WHERE r.role_id = ?
    ]], role_id)

    return {
        role_id = role.role_id,
        name = role.role_name,
        level = role.level,
        hp = role.hp,
        mp = role.mp,
        hp_max = role.hp_max,
        mp_max = role.mp_max,
        attack = role.attack,
        defense = role.defense,
        speed = role.speed,
        critical_rate = role.critical_rate,
        critical_damage = role.critical_damage,
        buffs = {},
        cooldowns = {},
    }
end

-- 计算伤害
function M.calculate_damage(attacker, defender, skill)
    local base_damage = attacker.attack
    local damage_ratio = skill and skill.damage_ratio or 1.0

    -- 基础伤害
    local damage = base_damage * damage_ratio

    -- 防御减伤
    local defense_factor = 1 - (defender.defense / (defender.defense + 100))
    damage = damage * defense_factor

    -- 暴击
    local is_critical = false
    local crit_rate = attacker.critical_rate
    if skill and skill.crit_bonus then
        crit_rate = crit_rate + skill.crit_bonus
    end

    if math.random() < crit_rate then
        damage = damage * attacker.critical_damage
        is_critical = true
    end

    return math.floor(damage), is_critical
end

-- 施放技能
function M.cast_skill(battle_id, caster_id, skill_id, target_id)
    local battle = battles[battle_id]
    if not battle then
        return nil, "战斗不存在"
    end

    local skill = config.skill.get(skill_id)
    if not skill then
        return nil, "技能不存在"
    end

    -- 查找施法者
    local caster = M.find_unit(battle, caster_id)
    if not caster then
        return nil, "施法者不存在"
    end

    -- 检查MP
    if caster.mp < skill.mp_cost then
        return nil, "MP不足"
    end

    -- 检查冷却
    if caster.cooldowns[skill_id] and caster.cooldowns[skill_id] > utils.now() then
        return nil, "技能冷却中"
    end

    -- 消耗MP
    caster.mp = caster.mp - skill.mp_cost

    -- 设置冷却
    caster.cooldowns[skill_id] = utils.now() + skill.cooldown

    -- 查找目标
    local target = M.find_unit(battle, target_id)
    if not target then
        return nil, "目标不存在"
    end

    -- 计算伤害
    local damage, is_critical = M.calculate_damage(caster, target, skill)

    -- 应用伤害
    target.hp = math.max(0, target.hp - damage)

    log.info(string.format("Skill cast: %s -> %s, damage=%d, critical=%s",
        caster.name, target.name, damage, tostring(is_critical)))

    return {
        caster_id = caster_id,
        skill_id = skill_id,
        target_id = target_id,
        damage = damage,
        target_hp = target.hp,
        is_critical = is_critical,
    }
end

-- 查找单位
function M.find_unit(battle, role_id)
    for _, unit in ipairs(battle.attackers) do
        if unit.role_id == role_id then return unit end
    end
    for _, unit in ipairs(battle.defenders) do
        if unit.role_id == role_id then return unit end
    end
    return nil
end

-- 检查战斗结束
function M.check_battle_end(battle)
    local attackers_alive = false
    local defenders_alive = false

    for _, unit in ipairs(battle.attackers) do
        if unit.hp > 0 then attackers_alive = true end
    end

    for _, unit in ipairs(battle.defenders) do
        if unit.hp > 0 then defenders_alive = true end
    end

    if not attackers_alive then
        return 0  -- 防御方胜利
    elseif not defenders_alive then
        return 1  -- 攻击方胜利
    elseif battle.round >= battle.max_rounds then
        return 2  -- 平局
    end

    return -1  -- 战斗继续
end

-- 结束战斗
function M.end_battle(battle_id)
    local battle = battles[battle_id]
    if not battle then return end

    local result = M.check_battle_end(battle)

    -- 计算奖励
    local reward = nil
    if result == 1 then
        reward = M.calculate_reward(battle)
    end

    -- 清理
    battles[battle_id] = nil

    log.info("Battle ended: id=" .. battle_id .. ", result=" .. result)

    return {
        battle_id = battle_id,
        result = result,
        reward = reward,
    }
end

-- 计算奖励
function M.calculate_reward(battle)
    return {
        exp = 100,
        gold = 50,
        items = {
            { item_id = 1, count = 3 },
        }
    }
end

return M