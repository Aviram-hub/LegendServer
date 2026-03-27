/**
 * @file login_handler.h
 * @brief 登录消息处理器
 */

#pragma once

#include "types.h"
#include "noncopyable.h"

namespace legend {

class Session;
class Message;

/**
 * @brief 登录消息处理器
 */
class LoginHandler : public NonCopyable {
public:
    LoginHandler();
    ~LoginHandler();

    void handleLoginRequest(Session* session, const Message& msg);
    void handleLogoutRequest(Session* session, const Message& msg);
    void handleHeartbeat(Session* session, const Message& msg);
};

} // namespace legend