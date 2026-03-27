# LegendServer - 高性能MMORPG游戏服务器

## 项目简介

LegendServer是一个支持万人同时在线的MMORPG游戏服务器框架，采用C++/Lua混合编程架构，展示了以下技术能力：

- **高性能网络编程**: 基于epoll的Reactor模式，支持C10K并发
- **多线程架构**: 主从Reactor模式，线程池处理业务逻辑
- **C++/Lua混合编程**: 业务逻辑脚本化，支持热更新
- **数据库设计与优化**: MySQL连接池，Redis缓存，ORM封装

## 技术栈

| 组件 | 技术 |
|------|------|
| 开发语言 | C++17 + Lua 5.4 |
| 网络框架 | epoll + 非阻塞IO |
| 多线程 | Reactor模式 + 线程池 |
| 数据库 | MySQL 8.0 + Redis 6.x |
| 序列化 | Protobuf |
| 日志 | spdlog |
| 构建 | CMake |

## 项目结构

```
LegendServer/
├── src/                    # 源代码
│   ├── common/            # 公共模块(线程、网络基础、定时器等)
│   ├── core/              # 核心引擎(Reactor、消息系统、会话管理)
│   ├── lua/               # Lua脚本引擎
│   ├── database/          # 数据库层(MySQL、Redis、ORM)
│   ├── gateway/           # 网关服务器
│   └── game/              # 游戏服务器及业务模块
├── lua_scripts/           # Lua业务脚本
├── proto/                 # 协议定义
├── sql/                   # 数据库脚本
├── tests/                 # 测试代码
├── config/                # 配置文件
└── scripts/               # 运维脚本
```

## 核心功能

- 登录认证系统
- 角色管理系统
- 战斗系统
- 聊天系统
- 好友系统
- 组队系统

## 快速开始

### 编译

```bash
mkdir build && cd build
cmake .. && make -j$(nproc)
```

### 运行

```bash
./bin/legend_server -c ../config/server.yaml
```

### 测试

```bash
./bin/test_thread_pool
./bin/test_mysql_pool
```

## 性能指标

- 单机支持5000+并发连接
- QPS达到10万+
- 消息延迟<1ms
- CPU利用率90%+

## 文档

- [架构设计文档](docs/architecture.md)
- [协议设计文档](docs/protocol.md)
- [数据库设计文档](docs/database.md)

## 作者

作为展示C++/Lua混合编程、Linux高性能服务器开发能力的作品集项目。

## 许可证

MIT License