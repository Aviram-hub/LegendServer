-- 聊天模块

local log = _G.log
local utils = _G.utils
local config = _G.config

local M = {}

-- 频道类型
local CHANNEL = {
    WORLD = 0,
    PRIVATE = 1,
    TEAM = 2,
    GUILD = 3,
}

-- 聊天冷却
local cooldowns = {}

-- 初始化
function M.init()
    log.info("Chat module initialized")
end

-- 发送消息
function M.send_message(role_id, channel_type, to_role_id, content)
    -- 检查消息长度
    if #content > config.game.chat.max_length then
        return {
            code = 401,
            msg = "消息过长"
        }
    end

    -- 检查冷却
    local cd_key = role_id .. "_" .. channel_type
    if cooldowns[cd_key] and cooldowns[cd_key] > utils.now() then
        return {
            code = 400,
            msg = "发言过于频繁"
        }
    end

    -- 设置冷却
    local cd = 0
    if channel_type == CHANNEL.WORLD then
        cd = config.game.chat.world_cd
    elseif channel_type == CHANNEL.PRIVATE then
        cd = config.game.chat.private_cd
    end
    cooldowns[cd_key] = utils.now() + cd

    -- 获取发送者信息
    local role = db.query("SELECT role_name FROM player_role WHERE role_id = ?", role_id)

    -- 保存聊天记录
    db.execute([[
        INSERT INTO chat_log (channel_type, from_role_id, to_role_id, content)
        VALUES (?, ?, ?, ?)
    ]], channel_type, role_id, to_role_id or 0, content)

    -- 广播消息
    local message = {
        channel_type = channel_type,
        from_role_id = role_id,
        from_name = role.role_name,
        to_role_id = to_role_id,
        content = content,
        timestamp = utils.now(),
    }

    -- 根据频道类型广播
    if channel_type == CHANNEL.WORLD then
        M.broadcast_world(message)
    elseif channel_type == CHANNEL.PRIVATE then
        M.send_private(message, to_role_id)
    elseif channel_type == CHANNEL.TEAM then
        M.broadcast_team(message, role_id)
    elseif channel_type == CHANNEL.GUILD then
        M.broadcast_guild(message, role_id)
    end

    log.debug("Chat: " .. role.role_name .. ": " .. content)

    return {
        code = 0,
        msg = "发送成功"
    }
end

-- 世界频道广播
function M.broadcast_world(message)
    -- 通过C++广播到所有在线玩家
    broadcast_all("ChatNotify", message)
end

-- 私聊
function M.send_private(message, to_role_id)
    -- 检查目标是否在线
    if not M.is_online(to_role_id) then
        return
    end

    send_to_role(to_role_id, "ChatNotify", message)
end

-- 队伍频道广播
function M.broadcast_team(message, role_id)
    local team = db.query([[
        SELECT team_id FROM team_member WHERE role_id = ?
    ]], role_id)

    if not team then return end

    local members = db.query([[
        SELECT role_id FROM team_member WHERE team_id = ?
    ]], team.team_id)

    for _, member in ipairs(members) do
        send_to_role(member.role_id, "ChatNotify", message)
    end
end

-- 公会频道广播
function M.broadcast_guild(message, role_id)
    local guild = db.query([[
        SELECT guild_id FROM guild_member WHERE role_id = ?
    ]], role_id)

    if not guild then return end

    local members = db.query([[
        SELECT role_id FROM guild_member WHERE guild_id = ?
    ]], guild.guild_id)

    for _, member in ipairs(members) do
        send_to_role(member.role_id, "ChatNotify", message)
    end
end

-- 检查是否在线
function M.is_online(role_id)
    return redis.exists("online_role:" .. role_id)
end

-- 获取聊天记录
function M.get_history(channel_type, count, before_time)
    local records = db.query([[
        SELECT c.*, r.role_name as from_name
        FROM chat_log c
        LEFT JOIN player_role r ON c.from_role_id = r.role_id
        WHERE c.channel_type = ?
        AND c.send_time < ?
        ORDER BY c.send_time DESC
        LIMIT ?
    ]], channel_type, before_time or utils.now(), count or 20)

    local messages = {}
    for _, record in ipairs(records) do
        table.insert(messages, {
            channel_type = record.channel_type,
            from_role_id = record.from_role_id,
            from_name = record.from_name,
            to_role_id = record.to_role_id,
            content = record.content,
            timestamp = record.send_time,
        })
    end

    return messages
end

return M