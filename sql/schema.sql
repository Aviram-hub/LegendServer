-- LegendServer 数据库结构
-- 创建时间: 2024-01
-- 数据库: MySQL 8.0+

-- 创建数据库
CREATE DATABASE IF NOT EXISTS legend_db DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE legend_db;

-- 玩家账号表
CREATE TABLE IF NOT EXISTS player_account (
    account_id BIGINT PRIMARY KEY AUTO_INCREMENT,
    account_name VARCHAR(64) NOT NULL UNIQUE COMMENT '账号名',
    password_hash VARCHAR(64) NOT NULL COMMENT 'SHA256密码',
    salt VARCHAR(32) NOT NULL COMMENT '盐值',
    register_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '注册时间',
    last_login_time DATETIME COMMENT '最后登录时间',
    login_ip VARCHAR(45) COMMENT '登录IP',
    status TINYINT NOT NULL DEFAULT 0 COMMENT '状态: 0正常, 1封禁',
    created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_account_name (account_name),
    INDEX idx_login_time (last_login_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='玩家账号表';

-- 角色表
CREATE TABLE IF NOT EXISTS player_role (
    role_id BIGINT PRIMARY KEY AUTO_INCREMENT,
    account_id BIGINT NOT NULL COMMENT '账号ID',
    role_name VARCHAR(32) NOT NULL COMMENT '角色名',
    server_id INT NOT NULL DEFAULT 1 COMMENT '服务器ID',
    level INT NOT NULL DEFAULT 1 COMMENT '等级',
    exp BIGINT NOT NULL DEFAULT 0 COMMENT '经验',
    profession TINYINT NOT NULL DEFAULT 0 COMMENT '职业: 0战士, 1法师, 2弓手',
    gender TINYINT NOT NULL DEFAULT 0 COMMENT '性别: 0男, 1女',
    hp INT NOT NULL DEFAULT 100 COMMENT '当前血量',
    mp INT NOT NULL DEFAULT 100 COMMENT '当前魔法',
    gold BIGINT NOT NULL DEFAULT 0 COMMENT '金币',
    diamond INT NOT NULL DEFAULT 0 COMMENT '钻石',
    create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    last_update_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    UNIQUE KEY uk_account_server (account_id, server_id),
    UNIQUE KEY uk_role_name (role_name),
    INDEX idx_account_id (account_id),
    INDEX idx_level (level)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='角色表';

-- 角色属性表
CREATE TABLE IF NOT EXISTS role_attribute (
    role_id BIGINT PRIMARY KEY,
    attack INT NOT NULL DEFAULT 10 COMMENT '攻击力',
    defense INT NOT NULL DEFAULT 10 COMMENT '防御力',
    speed INT NOT NULL DEFAULT 10 COMMENT '速度',
    critical_rate DECIMAL(5,4) NOT NULL DEFAULT 0.05 COMMENT '暴击率',
    critical_damage DECIMAL(5,4) NOT NULL DEFAULT 1.5 COMMENT '暴击伤害倍率',
    hp_max INT NOT NULL DEFAULT 100 COMMENT '最大血量',
    mp_max INT NOT NULL DEFAULT 100 COMMENT '最大魔法',
    FOREIGN KEY (role_id) REFERENCES player_role(role_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='角色属性表';

-- 物品表
CREATE TABLE IF NOT EXISTS item (
    item_id BIGINT PRIMARY KEY AUTO_INCREMENT,
    role_id BIGINT NOT NULL COMMENT '角色ID',
    item_type INT NOT NULL COMMENT '物品类型ID',
    count INT NOT NULL DEFAULT 1 COMMENT '数量',
    bind TINYINT NOT NULL DEFAULT 0 COMMENT '绑定: 0未绑定, 1绑定',
    create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_role_id (role_id),
    INDEX idx_item_type (item_type)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='物品表';

-- 技能表
CREATE TABLE IF NOT EXISTS role_skill (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    role_id BIGINT NOT NULL COMMENT '角色ID',
    skill_id INT NOT NULL COMMENT '技能ID',
    level INT NOT NULL DEFAULT 1 COMMENT '技能等级',
    exp INT NOT NULL DEFAULT 0 COMMENT '技能经验',
    UNIQUE KEY uk_role_skill (role_id, skill_id),
    INDEX idx_role_id (role_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='技能表';

-- 好友关系表
CREATE TABLE IF NOT EXISTS friend_relation (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    role_id BIGINT NOT NULL COMMENT '角色ID',
    friend_role_id BIGINT NOT NULL COMMENT '好友角色ID',
    intimacy INT NOT NULL DEFAULT 0 COMMENT '亲密度',
    create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UNIQUE KEY uk_friend (role_id, friend_role_id),
    INDEX idx_friend_role (friend_role_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='好友关系表';

-- 聊天记录表
CREATE TABLE IF NOT EXISTS chat_log (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    channel_type TINYINT NOT NULL COMMENT '频道类型: 0世界, 1私聊, 2队伍, 3公会',
    from_role_id BIGINT NOT NULL COMMENT '发送者ID',
    to_role_id BIGINT COMMENT '接收者ID(私聊)',
    content VARCHAR(512) NOT NULL COMMENT '聊天内容',
    send_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '发送时间',
    INDEX idx_from_role (from_role_id),
    INDEX idx_to_role (to_role_id),
    INDEX idx_send_time (send_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='聊天记录表';

-- 组队表
CREATE TABLE IF NOT EXISTS team (
    team_id BIGINT PRIMARY KEY AUTO_INCREMENT,
    leader_role_id BIGINT NOT NULL COMMENT '队长角色ID',
    team_name VARCHAR(32) COMMENT '队伍名称',
    create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    status TINYINT NOT NULL DEFAULT 0 COMMENT '状态: 0正常, 1解散',
    INDEX idx_leader (leader_role_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='组队表';

-- 队员表
CREATE TABLE IF NOT EXISTS team_member (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    team_id BIGINT NOT NULL COMMENT '队伍ID',
    role_id BIGINT NOT NULL COMMENT '队员角色ID',
    join_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UNIQUE KEY uk_team_role (team_id, role_id),
    INDEX idx_role_id (role_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='队员表';

-- 战斗日志表
CREATE TABLE IF NOT EXISTS battle_log (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    battle_id BIGINT NOT NULL COMMENT '战斗ID',
    attacker_role_id BIGINT NOT NULL COMMENT '攻击者ID',
    defender_role_id BIGINT COMMENT '防御者ID',
    skill_id INT COMMENT '技能ID',
    damage INT COMMENT '伤害值',
    create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_battle_id (battle_id),
    INDEX idx_attacker (attacker_role_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='战斗日志表';

-- 公会表
CREATE TABLE IF NOT EXISTS guild (
    guild_id BIGINT PRIMARY KEY AUTO_INCREMENT,
    guild_name VARCHAR(32) NOT NULL UNIQUE COMMENT '公会名称',
    leader_role_id BIGINT NOT NULL COMMENT '会长角色ID',
    level INT NOT NULL DEFAULT 1 COMMENT '公会等级',
    member_count INT NOT NULL DEFAULT 1 COMMENT '成员数量',
    create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_leader (leader_role_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='公会表';

-- 公会成员表
CREATE TABLE IF NOT EXISTS guild_member (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    guild_id BIGINT NOT NULL COMMENT '公会ID',
    role_id BIGINT NOT NULL COMMENT '成员角色ID',
    position TINYINT NOT NULL DEFAULT 0 COMMENT '职位: 0成员, 1长老, 2副会长, 3会长',
    join_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UNIQUE KEY uk_role (role_id),
    INDEX idx_guild_id (guild_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='公会成员表';