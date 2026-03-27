-- 物品配置

local M = {
    -- 物品类型
    ITEM_TYPE = {
        CONSUMABLE = 1,  -- 消耗品
        EQUIPMENT = 2,   -- 装备
        MATERIAL = 3,    -- 材料
        QUEST = 4,       -- 任务物品
    },

    -- 品质
    QUALITY = {
        COMMON = 1,      -- 普通(白)
        UNCOMMON = 2,    -- 优秀(绿)
        RARE = 3,        -- 稀有(蓝)
        EPIC = 4,        -- 史诗(紫)
        LEGENDARY = 5,   -- 传说(橙)
    },

    -- 物品配置
    items = {
        -- 消耗品
        [1] = {
            id = 1,
            name = "生命药水",
            type = 1,
            quality = 1,
            max_stack = 99,
            hp_restore = 50,
            description = "恢复50点生命值",
        },
        [2] = {
            id = 2,
            name = "魔法药水",
            type = 1,
            quality = 1,
            max_stack = 99,
            mp_restore = 30,
            description = "恢复30点魔法值",
        },
        [3] = {
            id = 3,
            name = "高级生命药水",
            type = 1,
            quality = 2,
            max_stack = 99,
            hp_restore = 200,
            description = "恢复200点生命值",
        },

        -- 装备
        [101] = {
            id = 101,
            name = "新手剑",
            type = 2,
            quality = 1,
            slot = "weapon",
            attack = 10,
            level_require = 1,
            description = "新手使用的剑",
        },
        [102] = {
            id = 102,
            name = "铁剑",
            type = 2,
            quality = 2,
            slot = "weapon",
            attack = 25,
            level_require = 5,
            description = "铁制的剑",
        },
        [103] = {
            id = 103,
            name = "精钢剑",
            type = 2,
            quality = 3,
            slot = "weapon",
            attack = 50,
            critical_rate = 0.05,
            level_require = 10,
            description = "精钢打造的剑",
        },
        [201] = {
            id = 201,
            name = "新手护甲",
            type = 2,
            quality = 1,
            slot = "armor",
            defense = 5,
            hp_max = 20,
            level_require = 1,
            description = "新手护甲",
        },
        [202] = {
            id = 202,
            name = "铁甲",
            type = 2,
            quality = 2,
            slot = "armor",
            defense = 15,
            hp_max = 50,
            level_require = 5,
            description = "铁制护甲",
        },

        -- 材料
        [301] = {
            id = 301,
            name = "铁矿石",
            type = 3,
            quality = 1,
            max_stack = 999,
            description = "用于锻造的铁矿石",
        },
        [302] = {
            id = 302,
            name = "魔力水晶",
            type = 3,
            quality = 2,
            max_stack = 999,
            description = "蕴含魔力的水晶",
        },
    },
}

-- 获取物品配置
function M.get(item_id)
    return M.items[item_id]
end

-- 检查物品是否存在
function M.exists(item_id)
    return M.items[item_id] ~= nil
end

-- 获取物品名称
function M.get_name(item_id)
    local item = M.items[item_id]
    return item and item.name or "未知物品"
end

return M