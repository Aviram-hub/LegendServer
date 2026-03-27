/**
 * @file redis_pool.h
 * @brief Redis连接池
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "redis_client.h"
#include "common/thread/mutex.h"
#include "common/thread/condition.h"
#include <queue>

namespace legend {

/**
 * @brief Redis连接池配置
 */
struct RedisPoolConfig {
    String host = "127.0.0.1";
    uint16 port = 6379;
    String password = "";
    int database = 0;
    uint32 minConnections = 2;
    uint32 maxConnections = 10;
    uint32 connectTimeout = 3000;
};

/**
 * @brief Redis连接池
 */
class RedisPool : public NonCopyable {
public:
    explicit RedisPool(const RedisPoolConfig& config);
    ~RedisPool();

    // 初始化
    bool init();

    // 获取连接
    Ptr<RedisClient> getConnection(uint32 timeoutMs = 3000);

    // 归还连接
    void returnConnection(Ptr<RedisClient> client);

    // 关闭连接池
    void close();

    // 获取连接数
    size_t size() const;
    size_t availableSize() const;

private:
    Ptr<RedisClient> createConnection();

    RedisPoolConfig config_;
    std::queue<Ptr<RedisClient>> pool_;
    std::vector<Ptr<RedisClient>> allClients_;
    mutable Mutex mutex_;
    Condition notEmpty_;
    std::atomic<bool> running_;
};

/**
 * @brief Redis连接守卫
 */
class RedisGuard {
public:
    RedisGuard(RedisPool& pool, uint32 timeoutMs = 3000)
        : pool_(pool)
        , client_(pool.getConnection(timeoutMs)) {
    }

    ~RedisGuard() {
        if (client_) {
            pool_.returnConnection(client_);
        }
    }

    RedisClient* operator->() { return client_.get(); }
    RedisClient& operator*() { return *client_; }

    bool valid() const { return client_ && client_->connected(); }
    operator bool() const { return valid(); }

private:
    RedisPool& pool_;
    Ptr<RedisClient> client_;
};

} // namespace legend