/**
 * @file game_server.h
 * @brief 游戏服务器
 */

#pragma once

#include "types.h"
#include "noncopyable.h"
#include "core/server/server_config.h"
#include "lua/lua_engine.h"
#include "database/mysql/mysql_connection_pool.h"
#include "database/redis/redis_pool.h"
#include "database/orm/entity_manager.h"

namespace legend {

class EventLoop;

/**
 * @brief 游戏服务器
 */
class GameServer : public NonCopyable {
public:
    GameServer(EventLoop* loop, const ServerConfig& config);
    ~GameServer();

    bool init();
    bool start();
    void stop();

    EventLoop* loop() { return loop_; }
    LuaEngine* luaEngine() { return luaEngine_.get(); }
    MySQLConnectionPool* mysqlPool() { return mysqlPool_.get(); }
    RedisPool* redisPool() { return redisPool_.get(); }
    EntityManager* entityManager() { return entityManager_.get(); }

private:
    EventLoop* loop_;
    ServerConfig config_;

    UniquePtr<LuaEngine> luaEngine_;
    UniquePtr<MySQLConnectionPool> mysqlPool_;
    UniquePtr<RedisPool> redisPool_;
    UniquePtr<EntityManager> entityManager_;

    std::atomic<bool> running_;
};

} // namespace legend