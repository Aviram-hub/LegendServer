/**
 * @file login_handler.cpp
 * @brief 登录消息处理器实现
 */

#include "login_handler.h"
#include "core/session/session.h"
#include "core/message/message.h"

namespace legend {

LoginHandler::LoginHandler() = default;
LoginHandler::~LoginHandler() = default;

void LoginHandler::handleLoginRequest(Session* session, const Message& msg) {
    (void)session;
    (void)msg;
    // 处理登录请求
}

void LoginHandler::handleLogoutRequest(Session* session, const Message& msg) {
    (void)session;
    (void)msg;
    // 处理登出请求
}

void LoginHandler::handleHeartbeat(Session* session, const Message& msg) {
    (void)msg;
    if (session) {
        session->updateHeartbeat();
    }
}

} // namespace legend