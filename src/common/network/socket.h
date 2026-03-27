/**
 * @file socket.h
 * @brief Socket封装
 */

#pragma once

#include "types.h"
#include "noncopyable.h"

namespace legend {

/**
 * @brief Socket类
 */
class Socket : public NonCopyable {
public:
    explicit Socket(int sockfd);
    ~Socket();

    // 获取socket文件描述符
    int fd() const { return sockfd_; }

    // 绑定地址
    bool bind(const struct sockaddr_in& addr);

    // 监听
    bool listen(int backlog = 128);

    // 接受连接
    int accept(struct sockaddr_in* addr = nullptr);

    // 连接服务器
    bool connect(const struct sockaddr_in& addr);

    // 关闭写端
    void shutdownWrite();

    // 关闭socket
    void close();

    // 设置TCP_NODELAY (禁用Nagle算法)
    bool setTcpNoDelay(bool on = true);

    // 设置SO_REUSEADDR
    bool setReuseAddr(bool on = true);

    // 设置SO_REUSEPORT
    bool setReusePort(bool on = true);

    // 设置SO_KEEPALIVE
    bool setKeepAlive(bool on = true);

    // 设置非阻塞
    bool setNonBlocking(bool on = true);

    // 获取socket错误
    int getSocketError() const;

    // 是否是非阻塞
    bool isNonBlocking() const;

    // 创建TCP socket
    static int createTcpSocket();

    // 创建非阻塞TCP socket
    static int createNonBlockingTcpSocket();

    // 创建UDP socket
    static int createUdpSocket();

    // 创建非阻塞UDP socket
    static int createNonBlockingUdpSocket();

private:
    int sockfd_;
};

/**
 * @brief Socket工具函数
 */
namespace socket_util {

// 创建TCP连接
int tcpConnect(const char* ip, uint16 port);

// 创建TCP监听socket
int tcpListen(const char* ip, uint16 port, int backlog = 128);

// 获取本地地址
struct sockaddr_in getLocalAddr(int sockfd);

// 获取对端地址
struct sockaddr_in getPeerAddr(int sockfd);

// 地址转字符串
String toIp(const struct sockaddr_in& addr);
String toIpPort(const struct sockaddr_in& addr);

// 字符串转地址
bool fromIpPort(const char* ip, uint16 port, struct sockaddr_in* addr);

// 关闭socket
void close(int sockfd);

// 关闭写端
void shutdownWrite(int sockfd);

} // namespace socket_util

} // namespace legend