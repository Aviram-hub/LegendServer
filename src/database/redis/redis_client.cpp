/**
 * @file redis_client.cpp
 * @brief Redis客户端实现
 */

#include "redis_client.h"

namespace legend {

// RedisReply 实现
RedisReply::RedisReply()
    : reply_(nullptr) {
}

RedisReply::RedisReply(redisReply* reply)
    : reply_(reply) {
}

RedisReply::~RedisReply() {
    if (reply_) {
        freeReplyObject(reply_);
    }
}

RedisReply::RedisReply(RedisReply&& other) noexcept
    : reply_(other.reply_) {
    other.reply_ = nullptr;
}

RedisReply& RedisReply::operator=(RedisReply&& other) noexcept {
    if (this != &other) {
        if (reply_) {
            freeReplyObject(reply_);
        }
        reply_ = other.reply_;
        other.reply_ = nullptr;
    }
    return *this;
}

RedisReplyType RedisReply::type() const {
    if (!reply_) return RedisReplyType::NIL;

    switch (reply_->type) {
        case REDIS_REPLY_STRING:
            return RedisReplyType::STRING;
        case REDIS_REPLY_ARRAY:
            return RedisReplyType::ARRAY;
        case REDIS_REPLY_INTEGER:
            return RedisReplyType::INTEGER;
        case REDIS_REPLY_NIL:
            return RedisReplyType::NIL;
        case REDIS_REPLY_STATUS:
            return RedisReplyType::STATUS;
        case REDIS_REPLY_ERROR:
            return RedisReplyType::ERROR;
        default:
            return RedisReplyType::NIL;
    }
}

bool RedisReply::isNil() const {
    return !reply_ || reply_->type == REDIS_REPLY_NIL;
}

String RedisReply::asString() const {
    if (reply_ && reply_->type == REDIS_REPLY_STRING) {
        return String(reply_->str, reply_->len);
    }
    return "";
}

int64 RedisReply::asInteger() const {
    if (reply_ && reply_->type == REDIS_REPLY_INTEGER) {
        return reply_->integer;
    }
    return 0;
}

size_t RedisReply::arraySize() const {
    if (reply_ && reply_->type == REDIS_REPLY_ARRAY) {
        return reply_->elements;
    }
    return 0;
}

RedisReply RedisReply::operator[](size_t index) const {
    if (reply_ && reply_->type == REDIS_REPLY_ARRAY && index < reply_->elements) {
        return RedisReply(reinterpret_cast<redisReply*>(reply_->element[index]));
    }
    return RedisReply();
}

String RedisReply::errorMessage() const {
    if (reply_ && reply_->type == REDIS_REPLY_ERROR) {
        return String(reply_->str, reply_->len);
    }
    return "";
}

// RedisClient 实现
RedisClient::RedisClient()
    : context_(nullptr) {
}

RedisClient::~RedisClient() {
    disconnect();
}

bool RedisClient::connect(const String& host, uint16 port, uint32 timeoutMs) {
    struct timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

    context_ = redisConnectWithTimeout(host.c_str(), port, tv);
    if (!context_ || context_->err) {
        lastError_ = context_ ? context_->errstr : "Failed to connect";
        disconnect();
        return false;
    }

    return true;
}

bool RedisClient::connectUnix(const String& path, uint32 timeoutMs) {
    struct timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

    context_ = redisConnectUnixWithTimeout(path.c_str(), tv);
    if (!context_ || context_->err) {
        lastError_ = context_ ? context_->errstr : "Failed to connect";
        disconnect();
        return false;
    }

    return true;
}

void RedisClient::disconnect() {
    if (context_) {
        redisFree(context_);
        context_ = nullptr;
    }
}

RedisReply RedisClient::execute(const String& cmd) {
    if (!context_) return RedisReply();

    redisReply* reply = static_cast<redisReply*>(
        redisCommand(context_, cmd.c_str()));
    return RedisReply(reply);
}

RedisReply RedisClient::execute(const String& cmd, const std::vector<String>& args) {
    if (!context_) return RedisReply();

    std::vector<const char*> argv;
    std::vector<size_t> argvlen;

    argv.push_back(cmd.c_str());
    argvlen.push_back(cmd.size());

    for (const auto& arg : args) {
        argv.push_back(arg.c_str());
        argvlen.push_back(arg.size());
    }

    redisReply* reply = static_cast<redisReply*>(
        redisCommandArgv(context_, argv.size(), argv.data(), argvlen.data()));
    return RedisReply(reply);
}

bool RedisClient::set(const String& key, const String& value, uint32 expireSeconds) {
    RedisReply reply;
    if (expireSeconds > 0) {
        reply = execute("SET", {key, value, "EX", std::to_string(expireSeconds)});
    } else {
        reply = execute("SET", {key, value});
    }
    return reply.valid() && reply.type() == RedisReplyType::STATUS;
}

bool RedisClient::get(const String& key, String& value) {
    auto reply = execute("GET", {key});
    if (reply.valid() && !reply.isNil()) {
        value = reply.asString();
        return true;
    }
    return false;
}

bool RedisClient::del(const String& key) {
    auto reply = execute("DEL", {key});
    return reply.valid() && reply.asInteger() > 0;
}

bool RedisClient::exists(const String& key) {
    auto reply = execute("EXISTS", {key});
    return reply.valid() && reply.asInteger() > 0;
}

bool RedisClient::expire(const String& key, uint32 seconds) {
    auto reply = execute("EXPIRE", {key, std::to_string(seconds)});
    return reply.valid() && reply.asInteger() > 0;
}

int64 RedisClient::ttl(const String& key) {
    auto reply = execute("TTL", {key});
    return reply.valid() ? reply.asInteger() : -1;
}

bool RedisClient::hset(const String& key, const String& field, const String& value) {
    auto reply = execute("HSET", {key, field, value});
    return reply.valid();
}

bool RedisClient::hget(const String& key, const String& field, String& value) {
    auto reply = execute("HGET", {key, field});
    if (reply.valid() && !reply.isNil()) {
        value = reply.asString();
        return true;
    }
    return false;
}

bool RedisClient::hdel(const String& key, const String& field) {
    auto reply = execute("HDEL", {key, field});
    return reply.valid() && reply.asInteger() > 0;
}

bool RedisClient::hexists(const String& key, const String& field) {
    auto reply = execute("HEXISTS", {key, field});
    return reply.valid() && reply.asInteger() > 0;
}

int64 RedisClient::hlen(const String& key) {
    auto reply = execute("HLEN", {key});
    return reply.valid() ? reply.asInteger() : 0;
}

bool RedisClient::hgetall(const String& key, HashMap<String, String>& values) {
    auto reply = execute("HGETALL", {key});
    if (!reply.valid() || reply.type() != RedisReplyType::ARRAY) {
        return false;
    }

    size_t size = reply.arraySize();
    for (size_t i = 0; i + 1 < size; i += 2) {
        String field = reply[i].asString();
        String value = reply[i + 1].asString();
        values[field] = value;
    }
    return true;
}

int64 RedisClient::lpush(const String& key, const String& value) {
    auto reply = execute("LPUSH", {key, value});
    return reply.valid() ? reply.asInteger() : 0;
}

int64 RedisClient::rpush(const String& key, const String& value) {
    auto reply = execute("RPUSH", {key, value});
    return reply.valid() ? reply.asInteger() : 0;
}

bool RedisClient::lpop(const String& key, String& value) {
    auto reply = execute("LPOP", {key});
    if (reply.valid() && !reply.isNil()) {
        value = reply.asString();
        return true;
    }
    return false;
}

bool RedisClient::rpop(const String& key, String& value) {
    auto reply = execute("RPOP", {key});
    if (reply.valid() && !reply.isNil()) {
        value = reply.asString();
        return true;
    }
    return false;
}

int64 RedisClient::llen(const String& key) {
    auto reply = execute("LLEN", {key});
    return reply.valid() ? reply.asInteger() : 0;
}

bool RedisClient::lrange(const String& key, int64 start, int64 stop, std::vector<String>& values) {
    auto reply = execute("LRANGE", {key, std::to_string(start), std::to_string(stop)});
    if (!reply.valid() || reply.type() != RedisReplyType::ARRAY) {
        return false;
    }

    size_t size = reply.arraySize();
    for (size_t i = 0; i < size; ++i) {
        values.push_back(reply[i].asString());
    }
    return true;
}

int64 RedisClient::sadd(const String& key, const String& member) {
    auto reply = execute("SADD", {key, member});
    return reply.valid() ? reply.asInteger() : 0;
}

int64 RedisClient::srem(const String& key, const String& member) {
    auto reply = execute("SREM", {key, member});
    return reply.valid() ? reply.asInteger() : 0;
}

bool RedisClient::sismember(const String& key, const String& member) {
    auto reply = execute("SISMEMBER", {key, member});
    return reply.valid() && reply.asInteger() > 0;
}

int64 RedisClient::scard(const String& key) {
    auto reply = execute("SCARD", {key});
    return reply.valid() ? reply.asInteger() : 0;
}

bool RedisClient::smembers(const String& key, std::vector<String>& members) {
    auto reply = execute("SMEMBERS", {key});
    if (!reply.valid() || reply.type() != RedisReplyType::ARRAY) {
        return false;
    }

    size_t size = reply.arraySize();
    for (size_t i = 0; i < size; ++i) {
        members.push_back(reply[i].asString());
    }
    return true;
}

int64 RedisClient::zadd(const String& key, double score, const String& member) {
    auto reply = execute("ZADD", {key, std::to_string(score), member});
    return reply.valid() ? reply.asInteger() : 0;
}

int64 RedisClient::zrem(const String& key, const String& member) {
    auto reply = execute("ZREM", {key, member});
    return reply.valid() ? reply.asInteger() : 0;
}

double RedisClient::zscore(const String& key, const String& member) {
    auto reply = execute("ZSCORE", {key, member});
    if (reply.valid() && !reply.isNil()) {
        return std::stod(reply.asString());
    }
    return 0.0;
}

int64 RedisClient::zrank(const String& key, const String& member) {
    auto reply = execute("ZRANK", {key, member});
    return reply.valid() && !reply.isNil() ? reply.asInteger() : -1;
}

int64 RedisClient::zcard(const String& key) {
    auto reply = execute("ZCARD", {key});
    return reply.valid() ? reply.asInteger() : 0;
}

bool RedisClient::zrange(const String& key, int64 start, int64 stop, std::vector<String>& members) {
    auto reply = execute("ZRANGE", {key, std::to_string(start), std::to_string(stop)});
    if (!reply.valid() || reply.type() != RedisReplyType::ARRAY) {
        return false;
    }

    size_t size = reply.arraySize();
    for (size_t i = 0; i < size; ++i) {
        members.push_back(reply[i].asString());
    }
    return true;
}

bool RedisClient::zrangeWithScores(const String& key, int64 start, int64 stop,
                                    std::vector<std::pair<String, double>>& members) {
    auto reply = execute("ZRANGE", {key, std::to_string(start), std::to_string(stop), "WITHSCORES"});
    if (!reply.valid() || reply.type() != RedisReplyType::ARRAY) {
        return false;
    }

    size_t size = reply.arraySize();
    for (size_t i = 0; i + 1 < size; i += 2) {
        String member = reply[i].asString();
        double score = std::stod(reply[i + 1].asString());
        members.emplace_back(member, score);
    }
    return true;
}

bool RedisClient::publish(const String& channel, const String& message) {
    auto reply = execute("PUBLISH", {channel, message});
    return reply.valid();
}

bool RedisClient::subscribe(const String& channel) {
    auto reply = execute("SUBSCRIBE", {channel});
    return reply.valid();
}

bool RedisClient::unsubscribe(const String& channel) {
    auto reply = execute("UNSUBSCRIBE", {channel});
    return reply.valid();
}

bool RedisClient::multi() {
    auto reply = execute("MULTI");
    return reply.valid() && reply.type() == RedisReplyType::STATUS;
}

bool RedisClient::exec() {
    auto reply = execute("EXEC");
    return reply.valid() && reply.type() == RedisReplyType::ARRAY;
}

bool RedisClient::discard() {
    auto reply = execute("DISCARD");
    return reply.valid() && reply.type() == RedisReplyType::STATUS;
}

bool RedisClient::auth(const String& password) {
    auto reply = execute("AUTH", {password});
    return reply.valid() && reply.type() == RedisReplyType::STATUS;
}

bool RedisClient::select(int db) {
    auto reply = execute("SELECT", {std::to_string(db)});
    return reply.valid() && reply.type() == RedisReplyType::STATUS;
}

bool RedisClient::ping() {
    auto reply = execute("PING");
    return reply.valid() && reply.asString() == "PONG";
}

String RedisClient::errorMessage() const {
    if (context_ && context_->err) {
        return context_->errstr;
    }
    return lastError_;
}

} // namespace legend