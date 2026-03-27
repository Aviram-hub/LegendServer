-- LegendServer 主入口脚本
-- 加载所有模块

-- 设置搜索路径
package.path = "./lua_scripts/?.lua;./lua_scripts/?/init.lua;" .. package.path

-- 加载库
local logger = require("lib.logger")
local utils = require("lib.utils")

-- 加载配置
local server_config = require("config.server_config")
local skill_config = require("config.skill_config")
local item_config = require("config.item_config")

-- 加载模块
local login = require("modules.login")
local role = require("modules.role")
local battle = require("modules.battle")
local chat = require("modules.chat")
local friend = require("modules.friend")
local team = require("modules.team")

-- 导出全局变量
_G.log = logger
_G.utils = utils
_G.config = {
    server = server_config,
    skill = skill_config,
    item = item_config,
}

-- 模块表
local M = {
    login = login,
    role = role,
    battle = battle,
    chat = chat,
    friend = friend,
    team = team,
}

-- 初始化
function M.init()
    log.info("LegendServer Lua scripts initializing...")

    -- 初始化各模块
    login.init()
    role.init()
    battle.init()
    chat.init()
    friend.init()
    team.init()

    log.info("LegendServer Lua scripts initialized")
    return true
end

-- 热更新
function M.hot_reload(module_name)
    if module_name then
        package.loaded["modules." .. module_name] = nil
        local ok, mod = pcall(require, "modules." .. module_name)
        if ok then
            M[module_name] = mod
            log.info("Module " .. module_name .. " reloaded")
            return true
        else
            log.error("Failed to reload module " .. module_name .. ": " .. tostring(mod))
            return false
        end
    else
        -- 重新加载所有模块
        for name, _ in pairs(M) do
            if type(name) == "string" then
                M.hot_reload(name)
            end
        end
    end
end

return M