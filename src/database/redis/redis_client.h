/**
 * @file redis_client.h
 * @brief Redis客户端封装
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include <hiredis/hiredis.h>
#include <vector>

namespace legend {

/**
 * @brief Redis回复类型
 */
enum class RedisReplyType {
    STRING,
    ARRAY,
    INTEGER,
    NIL,
    STATUS,
    ERROR
};

/**
 * @brief Redis回复
 */
class RedisReply {
public:
    RedisReply();
    explicit RedisReply(redisReply* reply);
    ~RedisReply();

    RedisReply(const RedisReply&) = delete;
    RedisReply& operator=(const RedisReply&) = delete;

    RedisReply(RedisReply&& other) noexcept;
    RedisReply& operator=(RedisReply&& other) noexcept;

    // 是否有效
    bool valid() const { return reply_ != nullptr; }

    // 获取类型
    RedisReplyType type() const;

    // 是否为nil
    bool isNil() const;

    // 获取字符串
    String asString() const;

    // 获取整数
    int64 asInteger() const;

    // 获取数组大小
    size_t arraySize() const;

    // 获取数组元素
    RedisReply operator[](size_t index) const;

    // 获取错误信息
    String errorMessage() const;

private:
    redisReply* reply_;
};

/**
 * @brief Redis客户端
 */
class RedisClient : public NonCopyable {
public:
    RedisClient();
    ~RedisClient();

    // 连接Redis
    bool connect(const String& host, uint16 port, uint32 timeoutMs = 3000);
    bool connectUnix(const String& path, uint32 timeoutMs = 3000);

    // 断开连接
    void disconnect();

    // 是否已连接
    bool connected() const { return context_ != nullptr; }

    // 执行命令
    RedisReply execute(const String& cmd);
    RedisReply execute(const String& cmd, const std::vector<String>& args);

    // 字符串操作
    bool set(const String& key, const String& value, uint32 expireSeconds = 0);
    bool get(const String& key, String& value);
    bool del(const String& key);
    bool exists(const String& key);
    bool expire(const String& key, uint32 seconds);
    int64 ttl(const String& key);

    // 哈希操作
    bool hset(const String& key, const String& field, const String& value);
    bool hget(const String& key, const String& field, String& value);
    bool hdel(const String& key, const String& field);
    bool hexists(const String& key, const String& field);
    int64 hlen(const String& key);
    bool hgetall(const String& key, HashMap<String, String>& values);

    // 列表操作
    int64 lpush(const String& key, const String& value);
    int64 rpush(const String& key, const String& value);
    bool lpop(const String& key, String& value);
    bool rpop(const String& key, String& value);
    int64 llen(const String& key);
    bool lrange(const String& key, int64 start, int64 stop, std::vector<String>& values);

    // 集合操作
    int64 sadd(const String& key, const String& member);
    int64 srem(const String& key, const String& member);
    bool sismember(const String& key, const String& member);
    int64 scard(const String& key);
    bool smembers(const String& key, std::vector<String>& members);

    // 有序集合操作
    int64 zadd(const String& key, double score, const String& member);
    int64 zrem(const String& key, const String& member);
    double zscore(const String& key, const String& member);
    int64 zrank(const String& key, const String& member);
    int64 zcard(const String& key);
    bool zrange(const String& key, int64 start, int64 stop, std::vector<String>& members);
    bool zrangeWithScores(const String& key, int64 start, int64 stop,
                          std::vector<std::pair<String, double>>& members);

    // 发布订阅
    bool publish(const String& channel, const String& message);
    bool subscribe(const String& channel);
    bool unsubscribe(const String& channel);

    // 事务
    bool multi();
    bool exec();
    bool discard();

    // 认证
    bool auth(const String& password);

    // 选择数据库
    bool select(int db);

    // Ping
    bool ping();

    // 错误信息
    String errorMessage() const;

private:
    redisContext* context_;
    String lastError_;
};

} // namespace legend