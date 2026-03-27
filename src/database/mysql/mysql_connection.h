/**
 * @file mysql_connection.h
 * @brief MySQL连接封装
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include <mysql/mysql.h>
#include <string>
#include <vector>

namespace legend {

/**
 * @brief MySQL参数
 */
struct MySQLParam {
    enum Type {
        INT,
        UINT,
        FLOAT,
        DOUBLE,
        STRING,
        BLOB,
        NULL_VALUE
    };

    Type type;
    union {
        int64 intValue;
        uint64 uintValue;
        float floatValue;
        double doubleValue;
    };
    String stringValue;

    MySQLParam() : type(NULL_VALUE), intValue(0) {}
    explicit MySQLParam(int64 v) : type(INT), intValue(v) {}
    explicit MySQLParam(uint64 v) : type(UINT), uintValue(v) {}
    explicit MySQLParam(float v) : type(FLOAT), floatValue(v) {}
    explicit MySQLParam(double v) : type(DOUBLE), doubleValue(v) {}
    explicit MySQLParam(const String& v) : type(STRING), stringValue(v) {}
    explicit MySQLParam(const char* v) : type(STRING), stringValue(v) {}
};

/**
 * @brief MySQL结果集
 */
class MySQLResult {
public:
    MySQLResult();
    ~MySQLResult();

    // 是否有效
    bool valid() const { return result_ != nullptr; }

    // 获取行数
    uint64 rowCount() const;

    // 获取字段数
    uint32 fieldCount() const;

    // 获取字段名
    String fieldName(uint32 index) const;

    // 获取字段索引
    int32 fieldIndex(const String& name) const;

    // 下一行
    bool next();

    // 获取值
    int32 getInt32(uint32 index) const;
    int64 getInt64(uint32 index) const;
    uint32 getUInt32(uint32 index) const;
    uint64 getUInt64(uint32 index) const;
    float getFloat(uint32 index) const;
    double getDouble(uint32 index) const;
    String getString(uint32 index) const;
    String getBlob(uint32 index) const;
    bool isNull(uint32 index) const;

    // 按字段名获取
    int32 getInt32(const String& name) const;
    int64 getInt64(const String& name) const;
    uint32 getUInt32(const String& name) const;
    uint64 getUInt64(const String& name) const;
    float getFloat(const String& name) const;
    double getDouble(const String& name) const;
    String getString(const String& name) const;

private:
    friend class MySQLConnection;
    friend class MySQLStatement;

    void setResult(MYSQL_RES* result);
    void fetchFields();

    MYSQL_RES* result_;
    MYSQL_ROW row_;
    unsigned long* lengths_;
    std::vector<MYSQL_FIELD*> fields_;
    HashMap<String, uint32> fieldMap_;
};

/**
 * @brief MySQL连接
 */
class MySQLConnection : public NonCopyable {
public:
    MySQLConnection();
    ~MySQLConnection();

    // 连接数据库
    bool connect(const String& host, uint16 port,
                 const String& user, const String& password,
                 const String& database);

    // 断开连接
    void disconnect();

    // 是否已连接
    bool connected() const { return connected_; }

    // 执行查询
    MySQLResult executeQuery(const String& sql);

    // 执行预处理查询
    MySQLResult executeQuery(const String& sql, const std::vector<MySQLParam>& params);

    // 执行更新
    int64 executeUpdate(const String& sql);

    // 执行预处理更新
    int64 executeUpdate(const String& sql, const std::vector<MySQLParam>& params);

    // 插入并获取自增ID
    int64 insertAndGetId(const String& sql, const std::vector<MySQLParam>& params);

    // 事务
    bool beginTransaction();
    bool commit();
    bool rollback();

    // 错误信息
    int32 errorCode() const;
    String errorMessage() const;

    // 转义字符串
    String escapeString(const String& str);

    // 心跳检测
    bool ping();

private:
    MYSQL mysql_;
    bool connected_;
    bool inTransaction_;
};

} // namespace legend