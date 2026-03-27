/**
 * @file server_config.h
 * @brief 服务器配置
 */

#pragma once

#include "types.h"
#include "noncopyable.h"
#include <yaml-cpp/yaml.h>

namespace legend {

/**
 * @brief 服务器配置类
 */
class ServerConfig : public NonCopyable {
public:
    ServerConfig() = default;

    // 加载配置文件
    bool load(const String& filename);

    // 服务器配置
    const String& serverName() const { return serverName_; }
    const String& serverVersion() const { return serverVersion_; }

    // 网关配置
    uint16 gatewayPort() const { return gatewayPort_; }
    const String& gatewayHost() const { return gatewayHost_; }
    int maxConnections() const { return maxConnections_; }

    // 游戏服务器配置
    uint16 gamePort() const { return gamePort_; }
    const String& gameHost() const { return gameHost_; }
    int maxPlayers() const { return maxPlayers_; }

    // 线程配置
    int ioThreads() const { return ioThreads_; }
    int logicThreads() const { return logicThreads_; }
    int dbThreads() const { return dbThreads_; }

    // 数据库配置
    const String& mysqlHost() const { return mysqlHost_; }
    uint16 mysqlPort() const { return mysqlPort_; }
    const String& mysqlUser() const { return mysqlUser_; }
    const String& mysqlPassword() const { return mysqlPassword_; }
    const String& mysqlDatabase() const { return mysqlDatabase_; }
    int mysqlPoolSize() const { return mysqlPoolSize_; }

    const String& redisHost() const { return redisHost_; }
    uint16 redisPort() const { return redisPort_; }
    const String& redisPassword() const { return redisPassword_; }
    int redisDb() const { return redisDb_; }
    int redisPoolSize() const { return redisPoolSize_; }

    // 日志配置
    const String& logLevel() const { return logLevel_; }
    const String& logPath() const { return logPath_; }

    // 心跳配置
    int heartbeatInterval() const { return heartbeatInterval_; }
    int heartbeatTimeout() const { return heartbeatTimeout_; }

private:
    // 服务器信息
    String serverName_ = "LegendServer";
    String serverVersion_ = "1.0.0";

    // 网关配置
    String gatewayHost_ = "0.0.0.0";
    uint16 gatewayPort_ = 8888;
    int maxConnections_ = 10000;

    // 游戏服务器配置
    String gameHost_ = "0.0.0.0";
    uint16 gamePort_ = 8889;
    int maxPlayers_ = 5000;

    // 线程配置
    int ioThreads_ = 4;
    int logicThreads_ = 8;
    int dbThreads_ = 2;

    // MySQL配置
    String mysqlHost_ = "127.0.0.1";
    uint16 mysqlPort_ = 3306;
    String mysqlUser_ = "legend";
    String mysqlPassword_ = "legend123";
    String mysqlDatabase_ = "legend_db";
    int mysqlPoolSize_ = 20;

    // Redis配置
    String redisHost_ = "127.0.0.1";
    uint16 redisPort_ = 6379;
    String redisPassword_ = "";
    int redisDb_ = 0;
    int redisPoolSize_ = 10;

    // 日志配置
    String logLevel_ = "info";
    String logPath_ = "./logs";

    // 心跳配置
    int heartbeatInterval_ = 30000;
    int heartbeatTimeout_ = 90000;
};

} // namespace legend