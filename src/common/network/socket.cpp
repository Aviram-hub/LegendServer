/**
 * @file socket.cpp
 * @brief Socket实现
 */

#include "socket.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

namespace legend {

Socket::Socket(int sockfd)
    : sockfd_(sockfd) {
}

Socket::~Socket() {
    if (sockfd_ >= 0) {
        ::close(sockfd_);
    }
}

bool Socket::bind(const struct sockaddr_in& addr) {
    return ::bind(sockfd_, reinterpret_cast<const struct sockaddr*>(&addr),
                  sizeof(addr)) == 0;
}

bool Socket::listen(int backlog) {
    return ::listen(sockfd_, backlog) == 0;
}

int Socket::accept(struct sockaddr_in* addr) {
    socklen_t len = sizeof(*addr);
    return ::accept(sockfd_, reinterpret_cast<struct sockaddr*>(addr), &len);
}

bool Socket::connect(const struct sockaddr_in& addr) {
    return ::connect(sockfd_, reinterpret_cast<const struct sockaddr*>(&addr),
                     sizeof(addr)) == 0;
}

void Socket::shutdownWrite() {
    ::shutdown(sockfd_, SHUT_WR);
}

void Socket::close() {
    if (sockfd_ >= 0) {
        ::close(sockfd_);
        sockfd_ = -1;
    }
}

bool Socket::setTcpNoDelay(bool on) {
    int optval = on ? 1 : 0;
    return ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
                        &optval, sizeof(optval)) == 0;
}

bool Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    return ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
                        &optval, sizeof(optval)) == 0;
}

bool Socket::setReusePort(bool on) {
    int optval = on ? 1 : 0;
    return ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                        &optval, sizeof(optval)) == 0;
}

bool Socket::setKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    return ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
                        &optval, sizeof(optval)) == 0;
}

bool Socket::setNonBlocking(bool on) {
    int flags = ::fcntl(sockfd_, F_GETFL, 0);
    if (flags < 0) {
        return false;
    }
    if (on) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    return ::fcntl(sockfd_, F_SETFL, flags) >= 0;
}

int Socket::getSocketError() const {
    int optval;
    socklen_t optlen = sizeof(optval);
    if (::getsockopt(sockfd_, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
        return errno;
    }
    return optval;
}

bool Socket::isNonBlocking() const {
    int flags = ::fcntl(sockfd_, F_GETFL, 0);
    return flags >= 0 && (flags & O_NONBLOCK);
}

int Socket::createTcpSocket() {
    return ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

int Socket::createNonBlockingTcpSocket() {
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    return sockfd;
}

int Socket::createUdpSocket() {
    return ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

int Socket::createNonBlockingUdpSocket() {
    return ::socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP);
}

namespace socket_util {

int tcpConnect(const char* ip, uint16 port) {
    int sockfd = Socket::createTcpSocket();
    if (sockfd < 0) {
        return -1;
    }

    struct sockaddr_in addr;
    fromIpPort(ip, port, &addr);

    if (::connect(sockfd, reinterpret_cast<const struct sockaddr*>(&addr),
                  sizeof(addr)) < 0) {
        ::close(sockfd);
        return -1;
    }

    return sockfd;
}

int tcpListen(const char* ip, uint16 port, int backlog) {
    int sockfd = Socket::createNonBlockingTcpSocket();
    if (sockfd < 0) {
        return -1;
    }

    Socket socket(sockfd);
    socket.setReuseAddr(true);
    socket.setReusePort(true);

    struct sockaddr_in addr;
    fromIpPort(ip, port, &addr);

    if (!socket.bind(addr) || !socket.listen(backlog)) {
        return -1;
    }

    return socket.release();  // 释放所有权
}

struct sockaddr_in getLocalAddr(int sockfd) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    ::getsockname(sockfd, reinterpret_cast<struct sockaddr*>(&addr), &len);
    return addr;
}

struct sockaddr_in getPeerAddr(int sockfd) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    ::getpeername(sockfd, reinterpret_cast<struct sockaddr*>(&addr), &len);
    return addr;
}

String toIp(const struct sockaddr_in& addr) {
    char buf[INET_ADDRSTRLEN];
    ::inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof(buf));
    return buf;
}

String toIpPort(const struct sockaddr_in& addr) {
    return toIp(addr) + ":" + std::to_string(ntohs(addr.sin_port));
}

bool fromIpPort(const char* ip, uint16 port, struct sockaddr_in* addr) {
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    return ::inet_pton(AF_INET, ip, &addr->sin_addr) > 0;
}

void close(int sockfd) {
    ::close(sockfd);
}

void shutdownWrite(int sockfd) {
    ::shutdown(sockfd, SHUT_WR);
}

} // namespace socket_util

} // namespace legend