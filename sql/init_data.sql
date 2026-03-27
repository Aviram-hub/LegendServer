-- LegendServer 初始数据
-- 创建时间: 2024-01

USE legend_db;

-- 插入测试账号
INSERT INTO player_account (account_name, password_hash, salt, status) VALUES
('test001', '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8', 'salt001', 0),
('test002', '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8', 'salt002', 0);

-- 插入测试角色
INSERT INTO player_role (account_id, role_name, profession, gender, level, hp, mp, gold, diamond) VALUES
(1, 'Warrior001', 0, 0, 10, 200, 50, 1000, 10),
(1, 'Mage001', 1, 1, 8, 100, 200, 500, 5),
(2, 'Archer001', 2, 0, 15, 150, 100, 2000, 20);

-- 插入角色属性
INSERT INTO role_attribute (role_id, attack, defense, speed, hp_max, mp_max) VALUES
(1, 50, 30, 10, 200, 50),
(2, 80, 15, 8, 100, 200),
(3, 60, 20, 15, 150, 100);

-- 插入技能
INSERT INTO role_skill (role_id, skill_id, level) VALUES
(1, 1001, 1),
(1, 1002, 2),
(2, 2001, 1),
(3, 3001, 3);

-- 插入好友关系
INSERT INTO friend_relation (role_id, friend_role_id, intimacy) VALUES
(1, 2, 100),
(1, 3, 50),
(2, 1, 100);

-- 插入物品
INSERT INTO item (role_id, item_type, count, bind) VALUES
(1, 1, 10, 0),
(1, 2, 5, 1),
(2, 3, 1, 0),
(3, 1, 20, 0);

-- 插入队伍
INSERT INTO team (team_id, leader_role_id, team_name) VALUES
(1, 1, 'TestTeam');

INSERT INTO team_member (team_id, role_id) VALUES
(1, 1),
(1, 2);

-- 插入公会
INSERT INTO guild (guild_id, guild_name, leader_role_id, level) VALUES
(1, 'LegendGuild', 1, 1);

INSERT INTO guild_member (guild_id, role_id, position) VALUES
(1, 1, 3),
(1, 2, 1),
(1, 3, 0);