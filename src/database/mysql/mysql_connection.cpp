/**
 * @file mysql_connection.cpp
 * @brief MySQL连接实现
 */

#include "mysql_connection.h"
#include <cstring>

namespace legend {

// MySQLResult 实现
MySQLResult::MySQLResult()
    : result_(nullptr)
    , row_(nullptr)
    , lengths_(nullptr) {
}

MySQLResult::~MySQLResult() {
    if (result_) {
        mysql_free_result(result_);
    }
}

uint64 MySQLResult::rowCount() const {
    return result_ ? mysql_num_rows(result_) : 0;
}

uint32 MySQLResult::fieldCount() const {
    return result_ ? mysql_num_fields(result_) : 0;
}

String MySQLResult::fieldName(uint32 index) const {
    if (index < fields_.size() && fields_[index]) {
        return fields_[index]->name;
    }
    return "";
}

int32 MySQLResult::fieldIndex(const String& name) const {
    auto it = fieldMap_.find(name);
    return it != fieldMap_.end() ? static_cast<int32>(it->second) : -1;
}

bool MySQLResult::next() {
    if (!result_) return false;

    row_ = mysql_fetch_row(result_);
    lengths_ = mysql_fetch_lengths(result_);
    return row_ != nullptr;
}

int32 MySQLResult::getInt32(uint32 index) const {
    return row_ && row_[index] ? std::stoi(row_[index]) : 0;
}

int64 MySQLResult::getInt64(uint32 index) const {
    return row_ && row_[index] ? std::stoll(row_[index]) : 0;
}

uint32 MySQLResult::getUInt32(uint32 index) const {
    return row_ && row_[index] ? std::stoul(row_[index]) : 0;
}

uint64 MySQLResult::getUInt64(uint32 index) const {
    return row_ && row_[index] ? std::stoull(row_[index]) : 0;
}

float MySQLResult::getFloat(uint32 index) const {
    return row_ && row_[index] ? std::stof(row_[index]) : 0.0f;
}

double MySQLResult::getDouble(uint32 index) const {
    return row_ && row_[index] ? std::stod(row_[index]) : 0.0;
}

String MySQLResult::getString(uint32 index) const {
    if (row_ && row_[index] && lengths_) {
        return String(row_[index], lengths_[index]);
    }
    return "";
}

String MySQLResult::getBlob(uint32 index) const {
    if (row_ && row_[index] && lengths_) {
        return String(row_[index], lengths_[index]);
    }
    return "";
}

bool MySQLResult::isNull(uint32 index) const {
    return !row_ || !row_[index];
}

int32 MySQLResult::getInt32(const String& name) const {
    int32 index = fieldIndex(name);
    return index >= 0 ? getInt32(index) : 0;
}

int64 MySQLResult::getInt64(const String& name) const {
    int32 index = fieldIndex(name);
    return index >= 0 ? getInt64(index) : 0;
}

uint32 MySQLResult::getUInt32(const String& name) const {
    int32 index = fieldIndex(name);
    return index >= 0 ? getUInt32(index) : 0;
}

uint64 MySQLResult::getUInt64(const String& name) const {
    int32 index = fieldIndex(name);
    return index >= 0 ? getUInt64(index) : 0;
}

float MySQLResult::getFloat(const String& name) const {
    int32 index = fieldIndex(name);
    return index >= 0 ? getFloat(index) : 0.0f;
}

double MySQLResult::getDouble(const String& name) const {
    int32 index = fieldIndex(name);
    return index >= 0 ? getDouble(index) : 0.0;
}

String MySQLResult::getString(const String& name) const {
    int32 index = fieldIndex(name);
    return index >= 0 ? getString(index) : "";
}

void MySQLResult::setResult(MYSQL_RES* result) {
    if (result_) {
        mysql_free_result(result_);
    }
    result_ = result;
    fetchFields();
}

void MySQLResult::fetchFields() {
    fields_.clear();
    fieldMap_.clear();

    if (!result_) return;

    uint32 count = mysql_num_fields(result_);
    MYSQL_FIELD* fields = mysql_fetch_fields(result_);

    for (uint32 i = 0; i < count; ++i) {
        fields_.push_back(&fields[i]);
        fieldMap_[fields[i].name] = i;
    }
}

// MySQLConnection 实现
MySQLConnection::MySQLConnection()
    : connected_(false)
    , inTransaction_(false) {
    mysql_init(&mysql_);
}

MySQLConnection::~MySQLConnection() {
    disconnect();
}

bool MySQLConnection::connect(const String& host, uint16 port,
                              const String& user, const String& password,
                              const String& database) {
    if (connected_) {
        disconnect();
    }

    mysql_options(&mysql_, MYSQL_OPT_CONNECT_TIMEOUT, "5");
    mysql_options(&mysql_, MYSQL_OPT_READ_TIMEOUT, "30");
    mysql_options(&mysql_, MYSQL_OPT_WRITE_TIMEOUT, "30");
    mysql_options(&mysql_, MYSQL_SET_CHARSET_NAME, "utf8mb4");

    if (!mysql_real_connect(&mysql_, host.c_str(), user.c_str(),
                            password.c_str(), database.c_str(),
                            port, nullptr, CLIENT_MULTI_STATEMENTS)) {
        return false;
    }

    connected_ = true;
    return true;
}

void MySQLConnection::disconnect() {
    if (connected_) {
        mysql_close(&mysql_);
        connected_ = false;
    }
}

MySQLResult MySQLConnection::executeQuery(const String& sql) {
    MySQLResult result;

    if (!connected_) {
        return result;
    }

    if (mysql_real_query(&mysql_, sql.c_str(), sql.size()) == 0) {
        MYSQL_RES* res = mysql_store_result(&mysql_);
        if (res) {
            result.setResult(res);
        }
    }

    return result;
}

MySQLResult MySQLConnection::executeQuery(const String& sql,
                                           const std::vector<MySQLParam>& params) {
    // 构建预处理查询
    MySQLStatement stmt(&mysql_, sql);
    if (!stmt.valid()) {
        return MySQLResult();
    }

    // 绑定参数
    for (size_t i = 0; i < params.size(); ++i) {
        const auto& param = params[i];
        switch (param.type) {
            case MySQLParam::INT:
                stmt.bindParam(i, param.intValue);
                break;
            case MySQLParam::UINT:
                stmt.bindParam(i, param.uintValue);
                break;
            case MySQLParam::FLOAT:
                stmt.bindParam(i, param.floatValue);
                break;
            case MySQLParam::DOUBLE:
                stmt.bindParam(i, param.doubleValue);
                break;
            case MySQLParam::STRING:
                stmt.bindParam(i, param.stringValue);
                break;
            case MySQLParam::NULL_VALUE:
                stmt.bindNull(i);
                break;
            default:
                break;
        }
    }

    if (!stmt.execute()) {
        return MySQLResult();
    }

    return stmt.getResult();
}

int64 MySQLConnection::executeUpdate(const String& sql) {
    if (!connected_) {
        return -1;
    }

    if (mysql_real_query(&mysql_, sql.c_str(), sql.size()) != 0) {
        return -1;
    }

    return mysql_affected_rows(&mysql_);
}

int64 MySQLConnection::executeUpdate(const String& sql,
                                      const std::vector<MySQLParam>& params) {
    MySQLStatement stmt(&mysql_, sql);
    if (!stmt.valid()) {
        return -1;
    }

    for (size_t i = 0; i < params.size(); ++i) {
        const auto& param = params[i];
        switch (param.type) {
            case MySQLParam::INT:
                stmt.bindParam(i, param.intValue);
                break;
            case MySQLParam::UINT:
                stmt.bindParam(i, param.uintValue);
                break;
            case MySQLParam::FLOAT:
                stmt.bindParam(i, param.floatValue);
                break;
            case MySQLParam::DOUBLE:
                stmt.bindParam(i, param.doubleValue);
                break;
            case MySQLParam::STRING:
                stmt.bindParam(i, param.stringValue);
                break;
            case MySQLParam::NULL_VALUE:
                stmt.bindNull(i);
                break;
            default:
                break;
        }
    }

    if (!stmt.execute()) {
        return -1;
    }

    return stmt.affectedRows();
}

int64 MySQLConnection::insertAndGetId(const String& sql,
                                       const std::vector<MySQLParam>& params) {
    MySQLStatement stmt(&mysql_, sql);
    if (!stmt.valid()) {
        return -1;
    }

    for (size_t i = 0; i < params.size(); ++i) {
        const auto& param = params[i];
        switch (param.type) {
            case MySQLParam::INT:
                stmt.bindParam(i, param.intValue);
                break;
            case MySQLParam::UINT:
                stmt.bindParam(i, param.uintValue);
                break;
            case MySQLParam::STRING:
                stmt.bindParam(i, param.stringValue);
                break;
            case MySQLParam::NULL_VALUE:
                stmt.bindNull(i);
                break;
            default:
                break;
        }
    }

    if (!stmt.execute()) {
        return -1;
    }

    return stmt.insertId();
}

bool MySQLConnection::beginTransaction() {
    if (!connected_ || inTransaction_) {
        return false;
    }

    if (mysql_autocommit(&mysql_, 0) != 0) {
        return false;
    }

    inTransaction_ = true;
    return true;
}

bool MySQLConnection::commit() {
    if (!connected_ || !inTransaction_) {
        return false;
    }

    if (mysql_commit(&mysql_) != 0) {
        return false;
    }

    mysql_autocommit(&mysql_, 1);
    inTransaction_ = false;
    return true;
}

bool MySQLConnection::rollback() {
    if (!connected_ || !inTransaction_) {
        return false;
    }

    if (mysql_rollback(&mysql_) != 0) {
        return false;
    }

    mysql_autocommit(&mysql_, 1);
    inTransaction_ = false;
    return true;
}

int32 MySQLConnection::errorCode() const {
    return mysql_errno(const_cast<MYSQL*>(&mysql_));
}

String MySQLConnection::errorMessage() const {
    return mysql_error(const_cast<MYSQL*>(&mysql_));
}

String MySQLConnection::escapeString(const String& str) {
    String result;
    result.resize(str.size() * 2 + 1);
    size_t len = mysql_real_escape_string(&mysql_, &result[0],
                                           str.c_str(), str.size());
    result.resize(len);
    return result;
}

bool MySQLConnection::ping() {
    return mysql_ping(&mysql_) == 0;
}

} // namespace legend