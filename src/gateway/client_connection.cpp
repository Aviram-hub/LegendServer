/**
 * @file client_connection.cpp
 * @brief 客户端连接实现
 */

#include "client_connection.h"

namespace legend {

ClientConnection::ClientConnection(int64 id, Ptr<Connection> conn)
    : id_(id)
    , connection_(conn)
    , lastHeartbeat_(nowMs()) {
}

ClientConnection::~ClientConnection() = default;

void ClientConnection::send(const void* data, size_t len) {
    if (connection_) {
        connection_->send(data, len);
    }
}

void ClientConnection::close() {
    if (connection_) {
        connection_->shutdown();
    }
}

} // namespace legend