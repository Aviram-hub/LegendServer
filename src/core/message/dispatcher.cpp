/**
 * @file dispatcher.cpp
 * @brief 消息分发器实现
 */

#include "dispatcher.h"
#include "core/session/session.h"

namespace legend {

MessageDispatcher& MessageDispatcher::instance() {
    static MessageDispatcher dispatcher;
    return dispatcher;
}

void MessageDispatcher::registerHandler(MessageId id, MessageHandler handler) {
    handlers_[id] = std::move(handler);
}

void MessageDispatcher::dispatch(Ptr<Session> session, const Message& msg) {
    auto it = handlers_.find(msg.messageId());
    if (it != handlers_.end()) {
        it->second(session, msg);
    }
}

bool MessageDispatcher::hasHandler(MessageId id) const {
    return handlers_.find(id) != handlers_.end();
}

} // namespace legend