/**
 * @file message_router.cpp
 * @brief 消息路由实现
 */

#include "message_router.h"
#include "core/session/session.h"
#include "core/message/message.h"

namespace legend {

MessageRouter& MessageRouter::instance() {
    static MessageRouter instance;
    return instance;
}

void MessageRouter::registerRoute(MessageId msgId, RouteHandler handler) {
    routes_[msgId] = std::move(handler);
}

void MessageRouter::route(Ptr<Session> session, const Message& msg) {
    auto it = routes_.find(msg.messageId());
    if (it != routes_.end()) {
        it->second(session, msg);
    }
}

} // namespace legend