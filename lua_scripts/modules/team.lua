-- 组队模块

local log = _G.log
local utils = _G.utils
local config = _G.config

local M = {}

-- 初始化
function M.init()
    log.info("Team module initialized")
end

-- 创建队伍
function M.create_team(leader_role_id, team_name)
    -- 检查是否已在队伍中
    local existing = db.query([[
        SELECT team_id FROM team_member WHERE role_id = ?
    ]], leader_role_id)

    if existing then
        return {
            code = 603,  -- ALREADY_IN_TEAM
            msg = "已在队伍中"
        }
    end

    -- 创建队伍
    local team_id = db.insert([[
        INSERT INTO team (leader_role_id, team_name)
        VALUES (?, ?)
    ]], leader_role_id, team_name or "")

    -- 添加队长为成员
    db.execute([[
        INSERT INTO team_member (team_id, role_id)
        VALUES (?, ?)
    ]], team_id, leader_role_id)

    log.info("Team created: id=" .. team_id .. ", leader=" .. leader_role_id)

    return {
        code = 0,
        team_id = team_id,
    }
end

-- 加入队伍
function M.join_team(team_id, role_id)
    -- 检查是否已在队伍中
    local existing = db.query([[
        SELECT team_id FROM team_member WHERE role_id = ?
    ]], role_id)

    if existing then
        return {
            code = 603,
            msg = "已在队伍中"
        }
    end

    -- 检查队伍是否存在且未满
    local team = db.query([[
        SELECT t.team_id, COUNT(m.role_id) as member_count
        FROM team t
        LEFT JOIN team_member m ON t.team_id = m.team_id
        WHERE t.team_id = ? AND t.status = 0
        GROUP BY t.team_id
    ]], team_id)

    if not team then
        return {
            code = 600,  -- TEAM_NOT_FOUND
            msg = "队伍不存在"
        }
    end

    if team.member_count >= config.game.team.max_members then
        return {
            code = 601,  -- TEAM_FULL
            msg = "队伍已满"
        }
    end

    -- 加入队伍
    db.execute([[
        INSERT INTO team_member (team_id, role_id)
        VALUES (?, ?)
    ]], team_id, role_id)

    log.info("Role joined team: team=" .. team_id .. ", role=" .. role_id)

    return {
        code = 0,
        msg = "加入成功"
    }
end

-- 离开队伍
function M.leave_team(role_id)
    local membership = db.query([[
        SELECT tm.team_id, t.leader_role_id
        FROM team_member tm
        LEFT JOIN team t ON tm.team_id = t.team_id
        WHERE tm.role_id = ?
    ]], role_id)

    if not membership then
        return {
            code = 602,  -- NOT_IN_TEAM
            msg = "不在队伍中"
        }
    end

    -- 删除成员
    db.execute([[
        DELETE FROM team_member WHERE role_id = ?
    ]], role_id)

    -- 如果是队长离开，转移队长或解散队伍
    if membership.leader_role_id == role_id then
        local new_leader = db.query([[
            SELECT role_id FROM team_member
            WHERE team_id = ?
            ORDER BY join_time ASC
            LIMIT 1
        ]], membership.team_id)

        if new_leader then
            -- 转移队长
            db.execute([[
                UPDATE team SET leader_role_id = ? WHERE team_id = ?
            ]], new_leader.role_id, membership.team_id)

            log.info("Team leader transferred: team=" .. membership.team_id ..
                     ", new_leader=" .. new_leader.role_id)
        else
            -- 解散队伍
            db.execute([[
                UPDATE team SET status = 1 WHERE team_id = ?
            ]], membership.team_id)

            log.info("Team dissolved: id=" .. membership.team_id)
        end
    end

    return {
        code = 0,
        msg = "离开成功"
    }
end

-- 踢出队伍
function M.kick_member(leader_role_id, target_role_id)
    -- 检查是否是队长
    local team = db.query([[
        SELECT team_id FROM team WHERE leader_role_id = ?
    ]], leader_role_id)

    if not team then
        return {
            code = 602,
            msg = "不是队长"
        }
    end

    -- 踢出成员
    db.execute([[
        DELETE FROM team_member WHERE team_id = ? AND role_id = ?
    ]], team.team_id, target_role_id)

    log.info("Member kicked: team=" .. team.team_id .. ", role=" .. target_role_id)

    return {
        code = 0,
        msg = "踢出成功"
    }
end

-- 获取队伍信息
function M.get_team_info(team_id)
    local team = db.query([[
        SELECT * FROM team WHERE team_id = ? AND status = 0
    ]], team_id)

    if not team then
        return nil
    end

    local members = db.query([[
        SELECT tm.role_id, tm.join_time, r.role_name, r.level, r.profession
        FROM team_member tm
        LEFT JOIN player_role r ON tm.role_id = r.role_id
        WHERE tm.team_id = ?
        ORDER BY tm.join_time ASC
    ]], team_id)

    team.members = members

    return team
end

-- 通过角色ID获取队伍信息
function M.get_team_by_role(role_id)
    local membership = db.query([[
        SELECT team_id FROM team_member WHERE role_id = ?
    ]], role_id)

    if not membership then
        return nil
    end

    return M.get_team_info(membership.team_id)
end

return M