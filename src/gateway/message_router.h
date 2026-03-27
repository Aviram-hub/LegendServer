/**
 * @file message_router.h
 * @brief 消息路由
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include <functional>

namespace legend {

class Message;
class Session;

/**
 * @brief 消息路由器
 */
class MessageRouter : public NonCopyable {
public:
    using RouteHandler = std::function<void(Ptr<Session>, const Message&)>;

    static MessageRouter& instance();

    // 注册路由
    void registerRoute(MessageId msgId, RouteHandler handler);

    // 路由消息
    void route(Ptr<Session> session, const Message& msg);

private:
    MessageRouter() = default;

    HashMap<MessageId, RouteHandler> routes_;
};

} // namespace legend