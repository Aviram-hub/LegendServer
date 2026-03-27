/**
 * @file server_manager.h
 * @brief 服务器管理器
 */

#pragma once

#include "types.h"
#include "noncopyable.h"

namespace legend {

/**
 * @brief 服务器管理器
 */
class ServerManager : public NonCopyable {
public:
    static ServerManager& instance();

    void init();
    void shutdown();

private:
    ServerManager() = default;
};

} // namespace legend