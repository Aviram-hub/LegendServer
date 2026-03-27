/**
 * @file mysql_statement.cpp
 * @brief MySQL预处理语句实现
 */

#include "mysql_statement.h"
#include <cstring>

namespace legend {

MySQLStatement::MySQLStatement(MYSQL* mysql, const String& sql)
    : stmt_(nullptr)
    , bindParams_(nullptr)
    , bindResult_(nullptr)
    , paramLengths_(nullptr)
    , resultLengths_(nullptr)
    , paramCount_(0)
    , fieldCount_(0) {
    stmt_ = mysql_stmt_init(mysql);
    if (!stmt_) {
        return;
    }

    if (mysql_stmt_prepare(stmt_, sql.c_str(), sql.size()) != 0) {
        mysql_stmt_close(stmt_);
        stmt_ = nullptr;
        return;
    }

    paramCount_ = mysql_stmt_param_count(stmt_);
    if (paramCount_ > 0) {
        bindParams_ = new MYSQL_BIND[paramCount_];
        paramLengths_ = new unsigned long[paramCount_];
        memset(bindParams_, 0, sizeof(MYSQL_BIND) * paramCount_);
    }

    MYSQL_RES* result = mysql_stmt_result_metadata(stmt_);
    if (result) {
        fieldCount_ = mysql_num_fields(result);
        if (fieldCount_ > 0) {
            bindResult_ = new MYSQL_BIND[fieldCount_];
            resultLengths_ = new unsigned long[fieldCount_];
            memset(bindResult_, 0, sizeof(MYSQL_BIND) * fieldCount_);
        }
        mysql_free_result(result);
    }
}

MySQLStatement::~MySQLStatement() {
    if (stmt_) {
        mysql_stmt_close(stmt_);
    }
    delete[] bindParams_;
    delete[] bindResult_;
    delete[] paramLengths_;
    delete[] resultLengths_;
}

bool MySQLStatement::bindParam(uint32 index, int32 value) {
    if (index >= paramCount_) return false;
    bindParams_[index].buffer_type = MYSQL_TYPE_LONG;
    bindParams_[index].buffer = new int32(value);
    bindParams_[index].is_unsigned = false;
    return mysql_stmt_bind_param(stmt_, bindParams_) == 0;
}

bool MySQLStatement::bindParam(uint32 index, int64 value) {
    if (index >= paramCount_) return false;
    bindParams_[index].buffer_type = MYSQL_TYPE_LONGLONG;
    bindParams_[index].buffer = new int64(value);
    bindParams_[index].is_unsigned = false;
    return mysql_stmt_bind_param(stmt_, bindParams_) == 0;
}

bool MySQLStatement::bindParam(uint32 index, uint32 value) {
    if (index >= paramCount_) return false;
    bindParams_[index].buffer_type = MYSQL_TYPE_LONG;
    bindParams_[index].buffer = new uint32(value);
    bindParams_[index].is_unsigned = true;
    return mysql_stmt_bind_param(stmt_, bindParams_) == 0;
}

bool MySQLStatement::bindParam(uint32 index, uint64 value) {
    if (index >= paramCount_) return false;
    bindParams_[index].buffer_type = MYSQL_TYPE_LONGLONG;
    bindParams_[index].buffer = new uint64(value);
    bindParams_[index].is_unsigned = true;
    return mysql_stmt_bind_param(stmt_, bindParams_) == 0;
}

bool MySQLStatement::bindParam(uint32 index, float value) {
    if (index >= paramCount_) return false;
    bindParams_[index].buffer_type = MYSQL_TYPE_FLOAT;
    bindParams_[index].buffer = new float(value);
    return mysql_stmt_bind_param(stmt_, bindParams_) == 0;
}

bool MySQLStatement::bindParam(uint32 index, double value) {
    if (index >= paramCount_) return false;
    bindParams_[index].buffer_type = MYSQL_TYPE_DOUBLE;
    bindParams_[index].buffer = new double(value);
    return mysql_stmt_bind_param(stmt_, bindParams_) == 0;
}

bool MySQLStatement::bindParam(uint32 index, const String& value) {
    return bindParam(index, value.c_str(), value.size());
}

bool MySQLStatement::bindParam(uint32 index, const char* value) {
    return bindParam(index, value, strlen(value));
}

bool MySQLStatement::bindParam(uint32 index, const void* data, size_t len) {
    if (index >= paramCount_) return false;
    bindParams_[index].buffer_type = MYSQL_TYPE_VAR_STRING;
    bindParams_[index].buffer = const_cast<void*>(data);
    bindParams_[index].buffer_length = len;
    paramLengths_[index] = len;
    bindParams_[index].length = &paramLengths_[index];
    return mysql_stmt_bind_param(stmt_, bindParams_) == 0;
}

bool MySQLStatement::bindNull(uint32 index) {
    if (index >= paramCount_) return false;
    bindParams_[index].buffer_type = MYSQL_TYPE_NULL;
    bindParams_[index].is_null_value = 1;
    return mysql_stmt_bind_param(stmt_, bindParams_) == 0;
}

bool MySQLStatement::execute() {
    return mysql_stmt_execute(stmt_) == 0;
}

MySQLResult MySQLStatement::getResult() {
    MySQLResult result;

    if (fieldCount_ == 0) {
        return result;
    }

    MYSQL_RES* res = mysql_stmt_result_metadata(stmt_);
    if (!res) {
        return result;
    }

    // 绑定结果缓冲区
    // 简化实现，实际应该为每个字段分配缓冲区
    mysql_stmt_store_result(stmt_);

    result.setResult(res);
    return result;
}

uint64 MySQLStatement::affectedRows() const {
    return mysql_stmt_affected_rows(stmt_);
}

uint64 MySQLStatement::insertId() const {
    return mysql_stmt_insert_id(stmt_);
}

int32 MySQLStatement::errorCode() const {
    return mysql_stmt_errno(stmt_);
}

String MySQLStatement::errorMessage() const {
    return mysql_stmt_error(stmt_);
}

void MySQLStatement::reset() {
    mysql_stmt_reset(stmt_);
}

} // namespace legend