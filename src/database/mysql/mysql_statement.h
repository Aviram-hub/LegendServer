/**
 * @file mysql_statement.h
 * @brief MySQL预处理语句
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "database/mysql/mysql_connection.h"
#include <mysql/mysql.h>

namespace legend {

/**
 * @brief MySQL预处理语句
 */
class MySQLStatement : public NonCopyable {
public:
    MySQLStatement(MYSQL* mysql, const String& sql);
    ~MySQLStatement();

    // 是否有效
    bool valid() const { return stmt_ != nullptr; }

    // 绑定参数
    bool bindParam(uint32 index, int32 value);
    bool bindParam(uint32 index, int64 value);
    bool bindParam(uint32 index, uint32 value);
    bool bindParam(uint32 index, uint64 value);
    bool bindParam(uint32 index, float value);
    bool bindParam(uint32 index, double value);
    bool bindParam(uint32 index, const String& value);
    bool bindParam(uint32 index, const char* value);
    bool bindParam(uint32 index, const void* data, size_t len);
    bool bindNull(uint32 index);

    // 执行
    bool execute();

    // 获取结果
    MySQLResult getResult();

    // 获取影响的行数
    uint64 affectedRows() const;

    // 获取自增ID
    uint64 insertId() const;

    // 错误信息
    int32 errorCode() const;
    String errorMessage() const;

private:
    void reset();

    MYSQL_STMT* stmt_;
    MYSQL_BIND* bindParams_;
    MYSQL_BIND* bindResult_;
    unsigned long* paramLengths_;
    unsigned long* resultLengths_;
    uint32 paramCount_;
    uint32 fieldCount_;
};

} // namespace legend