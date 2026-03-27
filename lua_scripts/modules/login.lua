-- 登录模块

local log = _G.log
local utils = _G.utils

local M = {}

-- 初始化
function M.init()
    log.info("Login module initialized")
end

-- 处理登录请求
function M.handle_login(account, password, device_id)
    log.info("Login request: account=" .. account)

    -- 验证账号密码
    -- 这里应该调用C++的数据库接口
    local player = db.query("SELECT * FROM player_account WHERE account_name = ?", account)

    if not player then
        return {
            code = 100,  -- ACCOUNT_NOT_FOUND
            msg = "账号不存在"
        }
    end

    -- 验证密码
    local hash = crypto.sha256(password .. player.salt)
    if hash ~= player.password_hash then
        return {
            code = 101,  -- PASSWORD_ERROR
            msg = "密码错误"
        }
    end

    -- 检查账号状态
    if player.status == 1 then
        return {
            code = 102,  -- ACCOUNT_BANNED
            msg = "账号已被封禁"
        }
    end

    -- 检查是否已登录
    local online = redis.get("online:" .. player.account_id)
    if online then
        -- 踢下线
        redis.publish("kick:" .. online, "repeat_login")
    end

    -- 更新登录时间
    db.execute("UPDATE player_account SET last_login_time = NOW(), login_ip = ? WHERE account_id = ?",
               get_client_ip(), player.account_id)

    -- 生成Token
    local token = crypto.uuid()
    redis.set("token:" .. token, player.account_id, 86400)  -- 24小时过期

    -- 获取角色列表
    local roles = db.query("SELECT role_id FROM player_role WHERE account_id = ?", player.account_id)

    local role_ids = {}
    for _, role in ipairs(roles) do
        table.insert(role_ids, role.role_id)
    end

    return {
        code = 0,
        account_id = player.account_id,
        token = token,
        expire_time = utils.now() + 86400,
        role_ids = role_ids,
    }
end

-- 验证Token
function M.verify_token(token, account_id)
    local stored_id = redis.get("token:" .. token)
    if not stored_id then
        return false
    end
    return tonumber(stored_id) == account_id
end

-- 登出
function M.handle_logout(account_id)
    log.info("Logout request: account_id=" .. account_id)

    -- 删除在线状态
    redis.del("online:" .. account_id)

    -- 清理Token
    -- 这里可以维护一个反向映射

    return {
        code = 0,
        msg = "登出成功"
    }
end

return M