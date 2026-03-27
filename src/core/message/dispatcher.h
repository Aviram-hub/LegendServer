/**
 * @file dispatcher.h
 * @brief 消息分发器
 */

#pragma once

#include "types.h"
#include "noncopyable.h"
#include "message.h"
#include <functional>

namespace legend {

class Session;
class Message;

/**
 * @brief 消息处理器类型
 */
using MessageHandler = std::function<void(Ptr<Session>, const Message&)>;

/**
 * @brief 消息分发器
 */
class MessageDispatcher : public NonCopyable {
public:
    static MessageDispatcher& instance();

    // 注册消息处理器
    void registerHandler(MessageId id, MessageHandler handler);

    // 分发消息
    void dispatch(Ptr<Session> session, const Message& msg);

    // 是否有处理器
    bool hasHandler(MessageId id) const;

private:
    MessageDispatcher() = default;

    HashMap<MessageId, MessageHandler> handlers_;
};

/**
 * @brief 消息处理器注册器
 */
class HandlerRegistrar {
public:
    HandlerRegistrar(MessageId id, MessageHandler handler) {
        MessageDispatcher::instance().registerHandler(id, std::move(handler));
    }
};

// 注册处理器的宏
#define REGISTER_HANDLER(id, handler) \
    static legend::HandlerRegistrar _handler_registrar_##id(id, handler)

} // namespace legend