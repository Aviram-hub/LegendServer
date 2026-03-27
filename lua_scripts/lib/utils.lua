-- 工具函数库

local M = {}

-- 深拷贝
function M.deepcopy(obj)
    if type(obj) ~= "table" then
        return obj
    end

    local copy = {}
    for k, v in pairs(obj) do
        copy[M.deepcopy(k)] = M.deepcopy(v)
    end
    return setmetatable(copy, getmetatable(obj))
end

-- 合并表
function M.merge(t1, t2)
    local result = M.deepcopy(t1)
    for k, v in pairs(t2) do
        result[k] = v
    end
    return result
end

-- 检查表是否包含键
function M.contains(t, key)
    return t[key] ~= nil
end

-- 获取表长度
function M.size(t)
    local count = 0
    for _ in pairs(t) do
        count = count + 1
    end
    return count
end

-- 表转字符串
function M.table_tostring(t, indent)
    indent = indent or ""
    local result = "{\n"
    for k, v in pairs(t) do
        local key = type(k) == "string" and string.format("%q", k) or tostring(k)
        local value
        if type(v) == "table" then
            value = M.table_tostring(v, indent .. "  ")
        elseif type(v) == "string" then
            value = string.format("%q", v)
        else
            value = tostring(v)
        end
        result = result .. indent .. "  [" .. key .. "] = " .. value .. ",\n"
    end
    return result .. indent .. "}"
end

-- 随机数
function M.random(min, max)
    if min and max then
        return math.random(min, max)
    elseif min then
        return math.random(1, min)
    else
        return math.random()
    end
end

-- 随机选择
function M.random_choice(t)
    local keys = {}
    for k in pairs(t) do
        table.insert(keys, k)
    end
    return t[keys[math.random(#keys)]]
end

-- 打乱数组
function M.shuffle(t)
    for i = #t, 2, -1 do
        local j = math.random(i)
        t[i], t[j] = t[j], t[i]
    end
    return t
end

-- 字符串分割
function M.split(str, sep)
    sep = sep or "%s"
    local result = {}
    for part in string.gmatch(str, "([^" .. sep .. "]+)") do
        table.insert(result, part)
    end
    return result
end

-- 字符串trim
function M.trim(str)
    return str:match("^%s*(.-)%s*$")
end

-- 格式化数字
function M.format_number(num)
    local formatted = tostring(num)
    local k
    while true do
        formatted, k = formatted:gsub("^(-?%d+)(%d%d%d)", "%1,%2")
        if k == 0 then break end
    end
    return formatted
end

-- 检查字符串是否以指定前缀开始
function M.startswith(str, prefix)
    return str:sub(1, #prefix) == prefix
end

-- 检查字符串是否以指定后缀结束
function M.endswith(str, suffix)
    return str:sub(-#suffix) == suffix
end

-- 时间格式化
function M.format_time(timestamp)
    return os.date("%Y-%m-%d %H:%M:%S", timestamp)
end

-- 获取当前时间戳(毫秒)
function M.now_ms()
    return os.time() * 1000
end

-- 获取当前时间戳(秒)
function M.now()
    return os.time()
end

return M