/**
 * @file mysql_connection_pool.cpp
 * @brief MySQL连接池实现
 */

#include "mysql_connection_pool.h"

namespace legend {

MySQLConnectionPool::MySQLConnectionPool(const MySQLPoolConfig& config)
    : config_(config)
    , running_(false) {
}

MySQLConnectionPool::~MySQLConnectionPool() {
    close();
}

bool MySQLConnectionPool::init() {
    MutexGuard guard(mutex_);

    if (running_) {
        return true;
    }

    // 创建最小数量的连接
    for (uint32 i = 0; i < config_.minConnections; ++i) {
        auto conn = createConnection();
        if (!conn) {
            return false;
        }
        pool_.push(conn);
        allConnections_.push_back(conn);
    }

    running_ = true;
    return true;
}

Ptr<MySQLConnection> MySQLConnectionPool::getConnection(uint32 timeoutMs) {
    MutexGuard guard(mutex_);

    if (!running_) {
        return nullptr;
    }

    // 等待可用连接
    while (pool_.empty()) {
        if (allConnections_.size() < config_.maxConnections) {
            auto conn = createConnection();
            if (conn) {
                allConnections_.push_back(conn);
                return conn;
            }
        }

        if (!notEmpty_.waitFor(mutex_, timeoutMs)) {
            return nullptr;
        }
    }

    auto conn = pool_.front();
    pool_.pop();

    // 检查连接是否有效
    if (!conn->ping()) {
        conn->disconnect();
        if (conn->connect(config_.host, config_.port, config_.user,
                          config_.password, config_.database)) {
            return conn;
        }
        return nullptr;
    }

    return conn;
}

void MySQLConnectionPool::returnConnection(Ptr<MySQLConnection> conn) {
    if (!conn) return;

    MutexGuard guard(mutex_);
    if (running_) {
        pool_.push(conn);
        notEmpty_.notify();
    }
}

void MySQLConnectionPool::checkConnections() {
    MutexGuard guard(mutex_);

    for (auto& conn : allConnections_) {
        if (!conn->ping()) {
            conn->disconnect();
            conn->connect(config_.host, config_.port, config_.user,
                          config_.password, config_.database);
        }
    }
}

void MySQLConnectionPool::close() {
    MutexGuard guard(mutex_);

    running_ = false;
    notEmpty_.notifyAll();

    while (!pool_.empty()) {
        pool_.pop();
    }

    allConnections_.clear();
}

size_t MySQLConnectionPool::size() const {
    MutexGuard guard(mutex_);
    return allConnections_.size();
}

size_t MySQLConnectionPool::availableSize() const {
    MutexGuard guard(mutex_);
    return pool_.size();
}

Ptr<MySQLConnection> MySQLConnectionPool::createConnection() {
    auto conn = std::make_shared<MySQLConnection>();
    if (conn->connect(config_.host, config_.port, config_.user,
                      config_.password, config_.database)) {
        return conn;
    }
    return nullptr;
}

} // namespace legend