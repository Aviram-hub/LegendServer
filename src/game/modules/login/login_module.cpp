/**
 * @file login_module.cpp
 * @brief 登录模块实现
 */

#include "login_module.h"
#include "core/session/session.h"
#include "database/mysql/mysql_connection_pool.h"
#include "database/redis/redis_pool.h"
#include "common/crypto/md5.h"
#include "common/crypto/random.h"

namespace legend {

LoginModule::LoginModule()
    : mysqlPool_(nullptr)
    , redisPool_(nullptr) {
}

LoginModule::~LoginModule() = default;

void LoginModule::init(MySQLConnectionPool* mysqlPool, RedisPool* redisPool) {
    mysqlPool_ = mysqlPool;
    redisPool_ = redisPool;
}

void LoginModule::handleLogin(Session* session, const String& account, const String& password) {
    if (!session || !mysqlPool_) {
        return;
    }

    MySQLConnectionGuard conn(*mysqlPool_);
    if (!conn) {
        return;
    }

    // 查询账号
    MySQLResult result = conn->executeQuery(
        "SELECT * FROM player_account WHERE account_name = ?",
        {MySQLParam(account)});

    if (!result.next()) {
        // 账号不存在
        return;
    }

    int64 accountId = result.getInt64("account_id");
    String passwordHash = result.getString("password_hash");
    String salt = result.getString("salt");
    int status = result.getInt32("status");

    // 检查账号状态
    if (status == 1) {
        // 账号被封禁
        return;
    }

    // 验证密码
    if (!checkPassword(password, passwordHash, salt)) {
        // 密码错误
        return;
    }

    // 生成Token
    String token = generateToken(accountId);

    // 保存到Redis
    RedisGuard redis(*redisPool_);
    if (redis) {
        redis->set("token:" + token, std::to_string(accountId), 86400);
        redis->set("online:" + std::to_string(accountId), "1", 86400);
    }

    // 更新登录时间
    conn->executeUpdate(
        "UPDATE player_account SET last_login_time = NOW() WHERE account_id = ?",
        {MySQLParam(accountId)});

    // 设置会话信息
    session->setPlayerId(accountId);
    session->setToken(token);
    session->setLoginTime(nowMs());
}

void LoginModule::handleLogout(Session* session) {
    if (!session) {
        return;
    }

    RedisGuard redis(*redisPool_);
    if (redis) {
        redis->del("online:" + std::to_string(session->playerId()));
        redis->del("token:" + session->token());
    }
}

bool LoginModule::verifyToken(const String& token, int64 accountId) {
    RedisGuard redis(*redisPool_);
    if (!redis) {
        return false;
    }

    String storedId;
    if (redis->get("token:" + token, storedId)) {
        return std::stoll(storedId) == accountId;
    }
    return false;
}

String LoginModule::generateToken(int64 accountId) {
    String uuid = ThreadSafeRandom::uuid();
    String data = std::to_string(accountId) + uuid + std::to_string(nowMs());
    return MD5::hexDigest(data);
}

String LoginModule::hashPassword(const String& password, const String& salt) {
    String data = password + salt;
    return MD5::hexDigest(data);
}

bool LoginModule::checkPassword(const String& password, const String& hash, const String& salt) {
    String computedHash = hashPassword(password, salt);
    return computedHash == hash;
}

} // namespace legend