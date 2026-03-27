/**
 * @file game_server.cpp
 * @brief 游戏服务器实现
 */

#include "game_server.h"
#include "core/reactor/event_loop.h"

namespace legend {

GameServer::GameServer(EventLoop* loop, const ServerConfig& config)
    : loop_(loop)
    , config_(config)
    , running_(false) {
}

GameServer::~GameServer() {
    stop();
}

bool GameServer::init() {
    // 初始化MySQL连接池
    MySQLPoolConfig mysqlConfig;
    mysqlConfig.host = config_.mysqlHost();
    mysqlConfig.port = config_.mysqlPort();
    mysqlConfig.user = config_.mysqlUser();
    mysqlConfig.password = config_.mysqlPassword();
    mysqlConfig.database = config_.mysqlDatabase();
    mysqlConfig.maxConnections = config_.mysqlPoolSize();

    mysqlPool_ = std::make_unique<MySQLConnectionPool>(mysqlConfig);
    if (!mysqlPool_->init()) {
        return false;
    }

    // 初始化Redis连接池
    RedisPoolConfig redisConfig;
    redisConfig.host = config_.redisHost();
    redisConfig.port = config_.redisPort();
    redisConfig.password = config_.redisPassword();
    redisConfig.database = config_.redisDb();
    redisConfig.maxConnections = config_.redisPoolSize();

    redisPool_ = std::make_unique<RedisPool>(redisConfig);
    if (!redisPool_->init()) {
        return false;
    }

    // 初始化实体管理器
    entityManager_ = std::make_unique<EntityManager>();
    if (!entityManager_->init(mysqlPool_.get())) {
        return false;
    }

    // 初始化Lua引擎
    luaEngine_ = std::make_unique<LuaEngine>();
    if (!luaEngine_->loadScript("lua_scripts/main.lua")) {
        return false;
    }

    return true;
}

bool GameServer::start() {
    running_ = true;
    return true;
}

void GameServer::stop() {
    running_ = false;

    if (mysqlPool_) {
        mysqlPool_->close();
    }

    if (redisPool_) {
        redisPool_->close();
    }
}

} // namespace legend