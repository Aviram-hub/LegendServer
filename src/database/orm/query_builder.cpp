/**
 * @file query_builder.cpp
 * @brief 查询构建器实现
 */

#include "query_builder.h"

namespace legend {

QueryBuilder::QueryBuilder()
    : type_(QueryType::SELECT) {
}

QueryBuilder& QueryBuilder::select(const String& columns) {
    type_ = QueryType::SELECT;
    columns_ = columns;
    return *this;
}

QueryBuilder& QueryBuilder::select(const std::vector<String>& columns) {
    type_ = QueryType::SELECT;
    columns_.clear();
    for (size_t i = 0; i < columns.size(); ++i) {
        if (i > 0) columns_ += ", ";
        columns_ += columns[i];
    }
    return *this;
}

QueryBuilder& QueryBuilder::from(const String& table) {
    table_ = table;
    return *this;
}

QueryBuilder& QueryBuilder::where(const String& condition) {
    whereClause_ = "WHERE " + condition;
    return *this;
}

QueryBuilder& QueryBuilder::where(const String& column, const String& op, const MySQLParam& value) {
    whereClause_ = "WHERE " + column + " " + op + " ?";
    params_.push_back(value);
    return *this;
}

QueryBuilder& QueryBuilder::whereEq(const String& column, const MySQLParam& value) {
    return where(column, "=", value);
}

QueryBuilder& QueryBuilder::whereIn(const String& column, const std::vector<MySQLParam>& values) {
    String placeholders;
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) placeholders += ", ";
        placeholders += "?";
        params_.push_back(values[i]);
    }
    whereClause_ = "WHERE " + column + " IN (" + placeholders + ")";
    return *this;
}

QueryBuilder& QueryBuilder::whereLike(const String& column, const String& pattern) {
    whereClause_ = "WHERE " + column + " LIKE ?";
    params_.push_back(MySQLParam(pattern));
    return *this;
}

QueryBuilder& QueryBuilder::whereNull(const String& column) {
    whereClause_ = "WHERE " + column + " IS NULL";
    return *this;
}

QueryBuilder& QueryBuilder::whereNotNull(const String& column) {
    whereClause_ = "WHERE " + column + " IS NOT NULL";
    return *this;
}

QueryBuilder& QueryBuilder::andWhere(const String& condition) {
    whereClause_ += " AND " + condition;
    return *this;
}

QueryBuilder& QueryBuilder::orWhere(const String& condition) {
    whereClause_ += " OR " + condition;
    return *this;
}

QueryBuilder& QueryBuilder::orderBy(const String& column, const String& direction) {
    orderByClause_ = "ORDER BY " + column + " " + direction;
    return *this;
}

QueryBuilder& QueryBuilder::limit(int count) {
    limitClause_ = "LIMIT " + std::to_string(count);
    return *this;
}

QueryBuilder& QueryBuilder::offset(int offset) {
    offsetClause_ = "OFFSET " + std::to_string(offset);
    return *this;
}

QueryBuilder& QueryBuilder::insert(const String& table) {
    type_ = QueryType::INSERT;
    table_ = table;
    return *this;
}

QueryBuilder& QueryBuilder::values(const HashMap<String, MySQLParam>& data) {
    data_ = data;
    return *this;
}

QueryBuilder& QueryBuilder::update(const String& table) {
    type_ = QueryType::UPDATE;
    table_ = table;
    return *this;
}

QueryBuilder& QueryBuilder::set(const HashMap<String, MySQLParam>& data) {
    data_ = data;
    return *this;
}

QueryBuilder& QueryBuilder::deleteFrom(const String& table) {
    type_ = QueryType::DELETE_;
    table_ = table;
    return *this;
}

String QueryBuilder::buildSelectSQL() const {
    std::ostringstream oss;
    oss << "SELECT " << (columns_.empty() ? "*" : columns_);
    oss << " FROM " << table_;
    if (!whereClause_.empty()) oss << " " << whereClause_;
    if (!orderByClause_.empty()) oss << " " << orderByClause_;
    if (!limitClause_.empty()) oss << " " << limitClause_;
    if (!offsetClause_.empty()) oss << " " << offsetClause_;
    return oss.str();
}

String QueryBuilder::buildInsertSQL() const {
    std::ostringstream oss;
    oss << "INSERT INTO " << table_ << " (";

    bool first = true;
    for (const auto& pair : data_) {
        if (!first) oss << ", ";
        oss << pair.first;
        first = false;
    }

    oss << ") VALUES (";

    first = true;
    for (const auto& pair : data_) {
        if (!first) oss << ", ";
        oss << "?";
        params_.push_back(pair.second);
        first = false;
    }

    oss << ")";
    return oss.str();
}

String QueryBuilder::buildUpdateSQL() const {
    std::ostringstream oss;
    oss << "UPDATE " << table_ << " SET ";

    bool first = true;
    for (const auto& pair : data_) {
        if (!first) oss << ", ";
        oss << pair.first << " = ?";
        params_.push_back(pair.second);
        first = false;
    }

    if (!whereClause_.empty()) oss << " " << whereClause_;
    return oss.str();
}

String QueryBuilder::buildDeleteSQL() const {
    std::ostringstream oss;
    oss << "DELETE FROM " << table_;
    if (!whereClause_.empty()) oss << " " << whereClause_;
    return oss.str();
}

void QueryBuilder::reset() {
    type_ = QueryType::SELECT;
    table_.clear();
    columns_.clear();
    whereClause_.clear();
    orderByClause_.clear();
    limitClause_.clear();
    offsetClause_.clear();
    data_.clear();
    params_.clear();
}

} // namespace legend