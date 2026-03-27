/**
 * @file address.h
 * @brief 网络地址封装
 */

#pragma once

#include "types.h"
#include <netinet/in.h>
#include <arpa/inet.h>

namespace legend {

/**
 * @brief IPv4地址类
 */
class IPv4Address {
public:
    IPv4Address();
    explicit IPv4Address(uint16 port);
    IPv4Address(const String& ip, uint16 port);
    explicit IPv4Address(const struct sockaddr_in& addr);

    // 获取地址结构
    const struct sockaddr_in& addr() const { return addr_; }
    struct sockaddr_in& addr() { return addr_; }

    // 获取sockaddr指针
    const struct sockaddr* sockaddr() const {
        return reinterpret_cast<const struct sockaddr*>(&addr_);
    }
    struct sockaddr* sockaddr() {
        return reinterpret_cast<struct sockaddr*>(&addr_);
    }

    // 获取地址长度
    socklen_t socklen() const { return sizeof(addr_); }

    // 获取IP
    String ip() const;

    // 获取端口
    uint16 port() const;

    // 获取IP:Port字符串
    String ipPort() const;

    // 设置地址
    void setAddr(const struct sockaddr_in& addr) { addr_ = addr; }

    // 是否是通配地址
    bool isWildcard() const {
        return addr_.sin_addr.s_addr == INADDR_ANY;
    }

    // 是否是回环地址
    bool isLoopback() const;

    // 是否是局域网地址
    bool isPrivateNetwork() const;

    // 解析地址字符串
    static bool parse(const String& ipPort, IPv4Address* addr);
    static bool parse(const String& ip, uint16 port, IPv4Address* addr);

private:
    struct sockaddr_in addr_;
};

/**
 * @brief IPv6地址类
 */
class IPv6Address {
public:
    IPv6Address();
    explicit IPv6Address(uint16 port);
    IPv6Address(const String& ip, uint16 port);
    explicit IPv6Address(const struct sockaddr_in6& addr);

    const struct sockaddr_in6& addr() const { return addr_; }
    struct sockaddr_in6& addr() { return addr_; }

    const struct sockaddr* sockaddr() const {
        return reinterpret_cast<const struct sockaddr*>(&addr_);
    }
    struct sockaddr* sockaddr() {
        return reinterpret_cast<struct sockaddr*>(&addr_);
    }

    socklen_t socklen() const { return sizeof(addr_); }

    String ip() const;
    uint16 port() const;
    String ipPort() const;

private:
    struct sockaddr_in6 addr_;
};

// 类型别名
using Address = IPv4Address;

} // namespace legend