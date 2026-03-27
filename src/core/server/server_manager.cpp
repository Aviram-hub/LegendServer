/**
 * @file server_manager.cpp
 * @brief 服务器管理器实现
 */

#include "server_manager.h"

namespace legend {

ServerManager& ServerManager::instance() {
    static ServerManager instance;
    return instance;
}

void ServerManager::init() {
}

void ServerManager::shutdown() {
}

} // namespace legend