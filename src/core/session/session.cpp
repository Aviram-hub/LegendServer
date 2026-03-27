/**
 * @file session.cpp
 * @brief 会话实现
 */

#include "session.h"
#include "core/message/message.h"

namespace legend {

Session::Session(int64 id)
    : id_(id)
    , state_(SessionState::kConnecting) {
    lastHeartbeat_ = nowMs();
}

Session::~Session() {
    if (connection_) {
        connection_->forceClose();
    }
}

void Session::send(const Message& msg) {
    if (connection_ && state_ != SessionState::kClosed) {
        Buffer buffer;
        msg.serialize(buffer);
        connection_->send(buffer);
    }
}

void Session::send(const void* data, size_t len) {
    if (connection_ && state_ != SessionState::kClosed) {
        connection_->send(data, len);
    }
}

void Session::close() {
    state_ = SessionState::kClosing;
    if (connection_) {
        connection_->shutdown();
    }
}

} // namespace legend