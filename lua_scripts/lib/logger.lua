-- 日志库

local M = {}

-- 日志级别
local LEVEL = {
    TRACE = 1,
    DEBUG = 2,
    INFO = 3,
    WARN = 4,
    ERROR = 5,
    FATAL = 6,
}

local level_names = {
    [1] = "TRACE",
    [2] = "DEBUG",
    [3] = "INFO",
    [4] = "WARN",
    [5] = "ERROR",
    [6] = "FATAL",
}

-- 当前日志级别
M.level = LEVEL.INFO

-- 格式化时间
local function format_time()
    return os.date("%Y-%m-%d %H:%M:%S")
end

-- 通用日志函数
local function log(level, ...)
    if level < M.level then
        return
    end

    local args = {...}
    local msg = ""
    for i, v in ipairs(args) do
        msg = msg .. tostring(v) .. (i < #args and " " or "")
    end

    local output = string.format("[%s] [%s] %s",
        format_time(),
        level_names[level],
        msg
    )

    print(output)
end

-- 日志方法
function M.trace(...)
    log(LEVEL.TRACE, ...)
end

function M.debug(...)
    log(LEVEL.DEBUG, ...)
end

function M.info(...)
    log(LEVEL.INFO, ...)
end

function M.warn(...)
    log(LEVEL.WARN, ...)
end

function M.error(...)
    log(LEVEL.ERROR, ...)
end

function M.fatal(...)
    log(LEVEL.FATAL, ...)
end

-- 设置日志级别
function M.setLevel(level)
    if type(level) == "string" then
        level = LEVEL[level:upper()] or LEVEL.INFO
    end
    M.level = level
end

return M