/**
 * @file server_manager.h
 * @brief 服务器管理器
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"

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