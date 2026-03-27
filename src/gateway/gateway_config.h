/**
 * @file gateway_config.h
 * @brief 网关配置
 */

#pragma once

#include "types.h"
#include "noncopyable.h"

namespace legend {

/**
 * @brief 网关配置类
 */
class GatewayConfig : public NonCopyable {
public:
    GatewayConfig() = default;

    // 监听地址
    const String& host() const { return host_; }
    void setHost(const String& host) { host_ = host; }

    // 监听端口
    uint16 port() const { return port_; }
    void setPort(uint16 port) { port_ = port; }

    // 最大连接数
    int maxConnections() const { return maxConnections_; }
    void setMaxConnections(int max) { maxConnections_ = max; }

    // 连接超时
    int timeout() const { return timeout_; }
    void setTimeout(int timeout) { timeout_ = timeout; }

private:
    String host_ = "0.0.0.0";
    uint16 port_ = 8888;
    int maxConnections_ = 10000;
    int timeout_ = 90000;  // 90秒
};

} // namespace legend