-- 角色模块

local log = _G.log
local utils = _G.utils
local config = _G.config

local M = {}

-- 职业配置
local PROFESSION = {
    WARRIOR = 0,
    MAGE = 1,
    ARCHER = 2,
}

-- 初始化
function M.init()
    log.info("Role module initialized")
end

-- 创建角色
function M.create_role(account_id, role_name, profession, gender)
    log.info("Create role: account=" .. account_id .. ", name=" .. role_name)

    -- 检查角色名长度
    if #role_name > config.game.role.max_name_length then
        return {
            code = 201,
            msg = "角色名过长"
        }
    end

    -- 检查角色名是否已存在
    local existing = db.query("SELECT role_id FROM player_role WHERE role_name = ?", role_name)
    if existing then
        return {
            code = 201,  -- ROLE_NAME_EXISTS
            msg = "角色名已存在"
        }
    end

    -- 检查角色数量
    local count = db.query("SELECT COUNT(*) as cnt FROM player_role WHERE account_id = ?", account_id)
    if count.cnt >= config.game.role.max_per_account then
        return {
            code = 202,  -- ROLE_LIMIT_REACHED
            msg = "角色数量已达上限"
        }
    end

    -- 创建角色
    local role_id = db.insert([[
        INSERT INTO player_role (account_id, role_name, profession, gender, hp, mp)
        VALUES (?, ?, ?, ?, ?, ?)
    ]], account_id, role_name, profession, gender,
        config.game.role.default_hp, config.game.role.default_mp)

    -- 创建角色属性
    local attrs = M.get_default_attributes(profession)
    db.execute([[
        INSERT INTO role_attribute (role_id, attack, defense, speed, hp_max, mp_max)
        VALUES (?, ?, ?, ?, ?, ?)
    ]], role_id, attrs.attack, attrs.defense, attrs.speed, attrs.hp_max, attrs.mp_max)

    return {
        code = 0,
        role_info = {
            role_id = role_id,
            role_name = role_name,
            level = 1,
            profession = profession,
            gender = gender,
            hp = config.game.role.default_hp,
            mp = config.game.role.default_mp,
        }
    }
end

-- 获取默认属性
function M.get_default_attributes(profession)
    if profession == PROFESSION.WARRIOR then
        return {
            attack = 30,
            defense = 20,
            speed = 10,
            hp_max = 150,
            mp_max = 50,
        }
    elseif profession == PROFESSION.MAGE then
        return {
            attack = 40,
            defense = 10,
            speed = 12,
            hp_max = 80,
            mp_max = 150,
        }
    else  -- ARCHER
        return {
            attack = 35,
            defense = 15,
            speed = 15,
            hp_max = 100,
            mp_max = 100,
        }
    end
end

-- 获取角色信息
function M.get_role_info(role_id)
    local role = db.query("SELECT * FROM player_role WHERE role_id = ?", role_id)
    if not role then
        return nil
    end

    local attrs = db.query("SELECT * FROM role_attribute WHERE role_id = ?", role_id)

    return {
        role_id = role.role_id,
        role_name = role.role_name,
        level = role.level,
        exp = role.exp,
        profession = role.profession,
        gender = role.gender,
        hp = role.hp,
        mp = role.mp,
        gold = role.gold,
        diamond = role.diamond,
        attributes = attrs or {},
    }
end

-- 更新角色属性
function M.update_attribute(role_id, attr_name, value)
    db.execute("UPDATE role_attribute SET " .. attr_name .. " = ? WHERE role_id = ?", value, role_id)
end

-- 增加经验
function M.add_exp(role_id, exp)
    local role = db.query("SELECT level, exp FROM player_role WHERE role_id = ?", role_id)
    if not role then return end

    local new_exp = role.exp + exp
    local level_up = false

    -- 检查升级
    while new_exp >= M.get_exp_for_level(role.level + 1) do
        new_exp = new_exp - M.get_exp_for_level(role.level + 1)
        role.level = role.level + 1
        level_up = true
    end

    db.execute("UPDATE player_role SET level = ?, exp = ? WHERE role_id = ?",
               role.level, new_exp, role_id)

    if level_up then
        -- 触发升级事件
        M.on_level_up(role_id, role.level)
    end

    return role.level, new_exp
end

-- 获取升级所需经验
function M.get_exp_for_level(level)
    return level * level * 100
end

-- 升级事件
function M.on_level_up(role_id, new_level)
    log.info("Role level up: role_id=" .. role_id .. ", level=" .. new_level)

    -- 增加属性
    db.execute([[
        UPDATE role_attribute
        SET attack = attack + 2,
            defense = defense + 1,
            hp_max = hp_max + 10,
            mp_max = mp_max + 5
        WHERE role_id = ?
    ]], role_id)
end

return M