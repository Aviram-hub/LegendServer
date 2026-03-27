/**
 * @file mysql_connection_pool.h
 * @brief MySQL连接池
 */

#pragma once

#include "types.h"
#include "noncopyable.h"
#include "mysql_connection.h"
#include "common/thread/mutex.h"
#include "common/thread/condition.h"
#include <queue>

namespace legend {

/**
 * @brief MySQL连接池配置
 */
struct MySQLPoolConfig {
    String host = "127.0.0.1";
    uint16 port = 3306;
    String user = "root";
    String password = "";
    String database = "test";
    uint32 minConnections = 2;
    uint32 maxConnections = 10;
    uint32 connectTimeout = 5000;
    uint32 queryTimeout = 30000;
};

/**
 * @brief MySQL连接池
 */
class MySQLConnectionPool : public NonCopyable {
public:
    explicit MySQLConnectionPool(const MySQLPoolConfig& config);
    ~MySQLConnectionPool();

    // 初始化连接池
    bool init();

    // 获取连接
    Ptr<MySQLConnection> getConnection(uint32 timeoutMs = 5000);

    // 归还连接
    void returnConnection(Ptr<MySQLConnection> conn);

    // 检查连接健康
    void checkConnections();

    // 关闭连接池
    void close();

    // 获取当前连接数
    size_t size() const;
    size_t availableSize() const;

private:
    Ptr<MySQLConnection> createConnection();

    MySQLPoolConfig config_;
    std::queue<Ptr<MySQLConnection>> pool_;
    std::vector<Ptr<MySQLConnection>> allConnections_;
    mutable Mutex mutex_;
    Condition notEmpty_;
    std::atomic<bool> running_;
};

/**
 * @brief MySQL连接守卫
 */
class MySQLConnectionGuard {
public:
    MySQLConnectionGuard(MySQLConnectionPool& pool, uint32 timeoutMs = 5000)
        : pool_(pool)
        , conn_(pool.getConnection(timeoutMs)) {
    }

    ~MySQLConnectionGuard() {
        if (conn_) {
            pool_.returnConnection(conn_);
        }
    }

    MySQLConnection* operator->() { return conn_.get(); }
    MySQLConnection& operator*() { return *conn_; }

    bool valid() const { return conn_ && conn_->connected(); }
    operator bool() const { return valid(); }

private:
    MySQLConnectionPool& pool_;
    Ptr<MySQLConnection> conn_;
};

} // namespace legend