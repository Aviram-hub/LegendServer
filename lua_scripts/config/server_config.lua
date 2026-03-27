-- 服务器配置

local M = {
    -- 服务器信息
    server = {
        name = "LegendServer",
        version = "1.0.0",
    },

    -- 网络配置
    network = {
        gateway_port = 8888,
        game_port = 8889,
        max_connections = 10000,
        timeout = 90000,  -- 心跳超时(ms)
    },

    -- 数据库配置
    database = {
        mysql = {
            host = "127.0.0.1",
            port = 3306,
            database = "legend_db",
            user = "legend",
            password = "legend123",
            pool_size = 20,
        },
        redis = {
            host = "127.0.0.1",
            port = 6379,
            db = 0,
            pool_size = 10,
        },
    },

    -- 游戏配置
    game = {
        -- 角色配置
        role = {
            max_name_length = 16,
            max_per_account = 3,
            default_hp = 100,
            default_mp = 100,
        },

        -- 战斗配置
        battle = {
            max_rounds = 30,
            skill_cd_base = 1000,
        },

        -- 聊天配置
        chat = {
            max_length = 256,
            world_cd = 5000,
            private_cd = 1000,
        },

        -- 好友配置
        friend = {
            max_count = 100,
        },

        -- 组队配置
        team = {
            max_members = 5,
        },
    },

    -- 日志配置
    log = {
        level = "info",
        path = "./logs",
        max_size = 104857600,  -- 100MB
        max_files = 10,
    },
}

return M