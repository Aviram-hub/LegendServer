/**
 * @file address.cpp
 * @brief 网络地址实现
 */

#include "address.h"
#include <cstring>

namespace legend {

IPv4Address::IPv4Address() {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = INADDR_ANY;
    addr_.sin_port = 0;
}

IPv4Address::IPv4Address(uint16 port) {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = INADDR_ANY;
    addr_.sin_port = htons(port);
}

IPv4Address::IPv4Address(const String& ip, uint16 port) {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    ::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
}

IPv4Address::IPv4Address(const struct sockaddr_in& addr)
    : addr_(addr) {
}

String IPv4Address::ip() const {
    char buf[INET_ADDRSTRLEN];
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    return buf;
}

uint16 IPv4Address::port() const {
    return ntohs(addr_.sin_port);
}

String IPv4Address::ipPort() const {
    return ip() + ":" + std::to_string(port());
}

bool IPv4Address::isLoopback() const {
    uint32 addr = ntohl(addr_.sin_addr.s_addr);
    return (addr >> 24) == 127;
}

bool IPv4Address::isPrivateNetwork() const {
    uint32 addr = ntohl(addr_.sin_addr.s_addr);
    // 10.0.0.0/8
    if ((addr >> 24) == 10) return true;
    // 172.16.0.0/12
    if ((addr >> 20) == 0xAC1) return true;
    // 192.168.0.0/16
    if ((addr >> 16) == 0xC0A8) return true;
    return false;
}

bool IPv4Address::parse(const String& ipPort, IPv4Address* addr) {
    size_t pos = ipPort.find(':');
    if (pos == String::npos) {
        return false;
    }
    String ip = ipPort.substr(0, pos);
    uint16 port = static_cast<uint16>(std::stoi(ipPort.substr(pos + 1)));
    return parse(ip, port, addr);
}

bool IPv4Address::parse(const String& ip, uint16 port, IPv4Address* addr) {
    if (!addr) return false;
    *addr = IPv4Address(ip, port);
    return true;
}

// IPv6Address implementation
IPv6Address::IPv6Address() {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin6_family = AF_INET6;
    addr_.sin6_port = 0;
}

IPv6Address::IPv6Address(uint16 port) {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin6_family = AF_INET6;
    addr_.sin6_port = htons(port);
}

IPv6Address::IPv6Address(const String& ip, uint16 port) {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin6_family = AF_INET6;
    addr_.sin6_port = htons(port);
    ::inet_pton(AF_INET6, ip.c_str(), &addr_.sin6_addr);
}

IPv6Address::IPv6Address(const struct sockaddr_in6& addr)
    : addr_(addr) {
}

String IPv6Address::ip() const {
    char buf[INET6_ADDRSTRLEN];
    ::inet_ntop(AF_INET6, &addr_.sin6_addr, buf, sizeof(buf));
    return buf;
}

uint16 IPv6Address::port() const {
    return ntohs(addr_.sin6_port);
}

String IPv6Address::ipPort() const {
    return "[" + ip() + "]:" + std::to_string(port());
}

} // namespace legend