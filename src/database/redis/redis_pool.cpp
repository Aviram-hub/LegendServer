/**
 * @file redis_pool.cpp
 * @brief Redis连接池实现
 */

#include "redis_pool.h"

namespace legend {

RedisPool::RedisPool(const RedisPoolConfig& config)
    : config_(config)
    , running_(false) {
}

RedisPool::~RedisPool() {
    close();
}

bool RedisPool::init() {
    MutexGuard guard(mutex_);

    if (running_) {
        return true;
    }

    for (uint32 i = 0; i < config_.minConnections; ++i) {
        auto client = createConnection();
        if (!client) {
            return false;
        }
        pool_.push(client);
        allClients_.push_back(client);
    }

    running_ = true;
    return true;
}

Ptr<RedisClient> RedisPool::getConnection(uint32 timeoutMs) {
    MutexGuard guard(mutex_);

    if (!running_) {
        return nullptr;
    }

    while (pool_.empty()) {
        if (allClients_.size() < config_.maxConnections) {
            auto client = createConnection();
            if (client) {
                allClients_.push_back(client);
                return client;
            }
        }

        if (!notEmpty_.waitFor(mutex_, timeoutMs)) {
            return nullptr;
        }
    }

    auto client = pool_.front();
    pool_.pop();

    if (!client->ping()) {
        client->disconnect();
        if (client->connect(config_.host, config_.port, config_.connectTimeout)) {
            if (!config_.password.empty()) {
                client->auth(config_.password);
            }
            if (config_.database > 0) {
                client->select(config_.database);
            }
            return client;
        }
        return nullptr;
    }

    return client;
}

void RedisPool::returnConnection(Ptr<RedisClient> client) {
    if (!client) return;

    MutexGuard guard(mutex_);
    if (running_) {
        pool_.push(client);
        notEmpty_.notify();
    }
}

void RedisPool::close() {
    MutexGuard guard(mutex_);

    running_ = false;
    notEmpty_.notifyAll();

    while (!pool_.empty()) {
        pool_.pop();
    }

    allClients_.clear();
}

size_t RedisPool::size() const {
    MutexGuard guard(mutex_);
    return allClients_.size();
}

size_t RedisPool::availableSize() const {
    MutexGuard guard(mutex_);
    return pool_.size();
}

Ptr<RedisClient> RedisPool::createConnection() {
    auto client = std::make_shared<RedisClient>();
    if (client->connect(config_.host, config_.port, config_.connectTimeout)) {
        if (!config_.password.empty()) {
            if (!client->auth(config_.password)) {
                return nullptr;
            }
        }
        if (config_.database > 0) {
            if (!client->select(config_.database)) {
                return nullptr;
            }
        }
        return client;
    }
    return nullptr;
}

} // namespace legend