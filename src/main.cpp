/**
 * @file main.cpp
 * @brief LegendServer 主入口
 */

#include "types.h"
#include "core/reactor/event_loop.h"
#include "core/server/server.h"
#include "core/server/server_config.h"
#include "lua/lua_engine.h"
#include "database/mysql/mysql_connection_pool.h"
#include "database/redis/redis_pool.h"
#include "common/crypto/random.h"

#include <iostream>
#include <csignal>
#include <cstdlib>

using namespace legend;

// 全局变量
EventLoop* g_eventLoop = nullptr;
Server* g_server = nullptr;

// 信号处理
void signalHandler(int sig) {
    std::cout << "Received signal " << sig << ", shutting down..." << std::endl;

    if (g_eventLoop) {
        g_eventLoop->quit();
    }
}

// 打印帮助信息
void printHelp(const char* progName) {
    std::cout << "Usage: " << progName << " [options]\n"
              << "Options:\n"
              << "  -c <config>   Configuration file path\n"
              << "  -h            Show this help message\n"
              << "  -v            Show version\n"
              << std::endl;
}

// 打印版本
void printVersion() {
    std::cout << "LegendServer v1.0.0" << std::endl;
    std::cout << "A high-performance MMORPG game server" << std::endl;
}

int main(int argc, char* argv[]) {
    // 解析命令行参数
    String configFile = "config/server.yaml";

    for (int i = 1; i < argc; ++i) {
        String arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printHelp(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "-c" && i + 1 < argc) {
            configFile = argv[++i];
        }
    }

    std::cout << "================================" << std::endl;
    std::cout << "    LegendServer v1.0.0         " << std::endl;
    std::cout << "    High-performance MMORPG     " << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;

    // 初始化随机数种子
    std::srand(std::time(nullptr));

    // 加载配置
    ServerConfig config;
    if (!config.load(configFile)) {
        std::cerr << "Failed to load config file: " << configFile << std::endl;
        return 1;
    }

    std::cout << "Config loaded from: " << configFile << std::endl;

    // 注册信号处理
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    try {
        // 创建事件循环
        EventLoop eventLoop;
        g_eventLoop = &eventLoop;

        // 创建服务器
        Server server(&eventLoop, config);
        g_server = &server;

        // 初始化服务器
        if (!server.init()) {
            std::cerr << "Failed to initialize server" << std::endl;
            return 1;
        }

        // 启动服务器
        if (!server.start()) {
            std::cerr << "Failed to start server" << std::endl;
            return 1;
        }

        std::cout << "Server started successfully!" << std::endl;
        std::cout << "Gateway listening on port: " << config.gatewayPort() << std::endl;
        std::cout << "Game server listening on port: " << config.gamePort() << std::endl;

        // 运行事件循环
        eventLoop.loop();

        // 停止服务器
        server.stop();

        std::cout << "Server stopped." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}