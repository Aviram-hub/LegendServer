-- 好友模块

local log = _G.log
local utils = _G.utils
local config = _G.config

local M = {}

-- 初始化
function M.init()
    log.info("Friend module initialized")
end

-- 添加好友
function M.add_friend(role_id, friend_role_id)
    -- 检查是否已经是好友
    local existing = db.query([[
        SELECT id FROM friend_relation
        WHERE role_id = ? AND friend_role_id = ?
    ]], role_id, friend_role_id)

    if existing then
        return {
            code = 502,  -- ALREADY_FRIEND
            msg = "已经是好友"
        }
    end

    -- 检查好友数量
    local count = db.query([[
        SELECT COUNT(*) as cnt FROM friend_relation WHERE role_id = ?
    ]], role_id)

    if count.cnt >= config.game.friend.max_count then
        return {
            code = 501,  -- FRIEND_LIMIT_REACHED
            msg = "好友数量已达上限"
        }
    end

    -- 添加好友关系(双向)
    db.execute([[
        INSERT INTO friend_relation (role_id, friend_role_id)
        VALUES (?, ?), (?, ?)
    ]], role_id, friend_role_id, friend_role_id, role_id)

    log.info("Friend added: " .. role_id .. " <-> " .. friend_role_id)

    return {
        code = 0,
        msg = "添加成功"
    }
end

-- 删除好友
function M.remove_friend(role_id, friend_role_id)
    db.execute([[
        DELETE FROM friend_relation
        WHERE (role_id = ? AND friend_role_id = ?)
           OR (role_id = ? AND friend_role_id = ?)
    ]], role_id, friend_role_id, friend_role_id, role_id)

    log.info("Friend removed: " .. role_id .. " <-> " .. friend_role_id)

    return {
        code = 0,
        msg = "删除成功"
    }
end

-- 获取好友列表
function M.get_friend_list(role_id)
    local friends = db.query([[
        SELECT f.friend_role_id, f.intimacy, r.role_name, r.level, r.profession
        FROM friend_relation f
        LEFT JOIN player_role r ON f.friend_role_id = r.role_id
        WHERE f.role_id = ?
        ORDER BY f.intimacy DESC
    ]], role_id)

    local result = {}
    for _, friend in ipairs(friends) do
        local online = redis.exists("online_role:" .. friend.friend_role_id)
        table.insert(result, {
            role_id = friend.friend_role_id,
            name = friend.role_name,
            level = friend.level,
            profession = friend.profession,
            intimacy = friend.intimacy,
            online = online,
        })
    end

    return result
end

-- 检查好友关系
function M.is_friend(role_id, friend_role_id)
    local relation = db.query([[
        SELECT id FROM friend_relation
        WHERE role_id = ? AND friend_role_id = ?
    ]], role_id, friend_role_id)

    return relation ~= nil
end

-- 增加亲密度
function M.add_intimacy(role_id, friend_role_id, value)
    db.execute([[
        UPDATE friend_relation
        SET intimacy = intimacy + ?
        WHERE role_id = ? AND friend_role_id = ?
    ]], value, role_id, friend_role_id)

    -- 双向增加
    db.execute([[
        UPDATE friend_relation
        SET intimacy = intimacy + ?
        WHERE role_id = ? AND friend_role_id = ?
    ]], value, friend_role_id, role_id)
end

return M