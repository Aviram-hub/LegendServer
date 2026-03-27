/**
 * @file server_config.cpp
 * @brief 服务器配置实现
 */

#include "server_config.h"
#include <fstream>

namespace legend {

bool ServerConfig::load(const String& filename) {
    try {
        YAML::Node config = YAML::LoadFile(filename);

        // 服务器配置
        if (config["server"]) {
            auto server = config["server"];
            if (server["name"]) {
                serverName_ = server["name"].as<String>();
            }
            if (server["version"]) {
                serverVersion_ = server["version"].as<String>();
            }
        }

        // 网关配置
        if (config["server"] && config["server"]["gateway"]) {
            auto gateway = config["server"]["gateway"];
            if (gateway["host"]) {
                gatewayHost_ = gateway["host"].as<String>();
            }
            if (gateway["port"]) {
                gatewayPort_ = gateway["port"].as<uint16>();
            }
            if (gateway["max_connections"]) {
                maxConnections_ = gateway["max_connections"].as<int>();
            }
        }

        // 游戏服务器配置
        if (config["server"] && config["server"]["game"]) {
            auto game = config["server"]["game"];
            if (game["host"]) {
                gameHost_ = game["host"].as<String>();
            }
            if (game["port"]) {
                gamePort_ = game["port"].as<uint16>();
            }
            if (game["max_players"]) {
                maxPlayers_ = game["max_players"].as<int>();
            }
        }

        // 线程配置
        if (config["server"] && config["server"]["threads"]) {
            auto threads = config["server"]["threads"];
            if (threads["io_threads"]) {
                ioThreads_ = threads["io_threads"].as<int>();
            }
            if (threads["logic_threads"]) {
                logicThreads_ = threads["logic_threads"].as<int>();
            }
            if (threads["db_threads"]) {
                dbThreads_ = threads["db_threads"].as<int>();
            }
        }

        // 数据库配置
        if (config["database"] && config["database"]["mysql"]) {
            auto mysql = config["database"]["mysql"];
            if (mysql["host"]) {
                mysqlHost_ = mysql["host"].as<String>();
            }
            if (mysql["port"]) {
                mysqlPort_ = mysql["port"].as<uint16>();
            }
            if (mysql["user"]) {
                mysqlUser_ = mysql["user"].as<String>();
            }
            if (mysql["password"]) {
                mysqlPassword_ = mysql["password"].as<String>();
            }
            if (mysql["database"]) {
                mysqlDatabase_ = mysql["database"].as<String>();
            }
            if (mysql["pool_size"]) {
                mysqlPoolSize_ = mysql["pool_size"].as<int>();
            }
        }

        if (config["database"] && config["database"]["redis"]) {
            auto redis = config["database"]["redis"];
            if (redis["host"]) {
                redisHost_ = redis["host"].as<String>();
            }
            if (redis["port"]) {
                redisPort_ = redis["port"].as<uint16>();
            }
            if (redis["password"]) {
                redisPassword_ = redis["password"].as<String>();
            }
            if (redis["db"]) {
                redisDb_ = redis["db"].as<int>();
            }
            if (redis["pool_size"]) {
                redisPoolSize_ = redis["pool_size"].as<int>();
            }
        }

        // 日志配置
        if (config["server"] && config["server"]["log"]) {
            auto log = config["server"]["log"];
            if (log["level"]) {
                logLevel_ = log["level"].as<String>();
            }
            if (log["path"]) {
                logPath_ = log["path"].as<String>();
            }
        }

        // 心跳配置
        if (config["heartbeat"]) {
            auto heartbeat = config["heartbeat"];
            if (heartbeat["interval"]) {
                heartbeatInterval_ = heartbeat["interval"].as<int>();
            }
            if (heartbeat["timeout"]) {
                heartbeatTimeout_ = heartbeat["timeout"].as<int>();
            }
        }

        return true;

    } catch (const YAML::Exception& e) {
        return false;
    }
}

} // namespace legend