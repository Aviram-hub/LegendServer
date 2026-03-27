/**
 * @file login_module.h
 * @brief 登录模块
 */

#pragma once

#include "types.h"
#include "noncopyable.h"

namespace legend {

class Session;
class MySQLConnectionPool;
class RedisPool;

/**
 * @brief 登录模块
 */
class LoginModule : public NonCopyable {
public:
    LoginModule();
    ~LoginModule();

    void init(MySQLConnectionPool* mysqlPool, RedisPool* redisPool);

    // 处理登录请求
    void handleLogin(Session* session, const String& account, const String& password);

    // 处理登出请求
    void handleLogout(Session* session);

    // 验证Token
    bool verifyToken(const String& token, int64 accountId);

private:
    String generateToken(int64 accountId);
    String hashPassword(const String& password, const String& salt);
    bool checkPassword(const String& password, const String& hash, const String& salt);

    MySQLConnectionPool* mysqlPool_;
    RedisPool* redisPool_;
};

} // namespace legend