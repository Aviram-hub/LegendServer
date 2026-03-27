/**
 * @file query_builder.h
 * @brief 查询构建器
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "database/mysql/mysql_connection.h"
#include <sstream>

namespace legend {

/**
 * @brief 查询构建器
 */
class QueryBuilder : public NonCopyable {
public:
    QueryBuilder();
    ~QueryBuilder() = default;

    // SELECT
    QueryBuilder& select(const String& columns = "*");
    QueryBuilder& select(const std::vector<String>& columns);

    // FROM
    QueryBuilder& from(const String& table);

    // WHERE
    QueryBuilder& where(const String& condition);
    QueryBuilder& where(const String& column, const String& op, const MySQLParam& value);
    QueryBuilder& whereEq(const String& column, const MySQLParam& value);
    QueryBuilder& whereIn(const String& column, const std::vector<MySQLParam>& values);
    QueryBuilder& whereLike(const String& column, const String& pattern);
    QueryBuilder& whereNull(const String& column);
    QueryBuilder& whereNotNull(const String& column);

    // AND/OR
    QueryBuilder& andWhere(const String& condition);
    QueryBuilder& orWhere(const String& condition);

    // ORDER BY
    QueryBuilder& orderBy(const String& column, const String& direction = "ASC");

    // LIMIT/OFFSET
    QueryBuilder& limit(int count);
    QueryBuilder& offset(int offset);

    // INSERT
    QueryBuilder& insert(const String& table);
    QueryBuilder& values(const HashMap<String, MySQLParam>& data);

    // UPDATE
    QueryBuilder& update(const String& table);
    QueryBuilder& set(const HashMap<String, MySQLParam>& data);

    // DELETE
    QueryBuilder& deleteFrom(const String& table);

    // 构建SQL
    String buildSelectSQL() const;
    String buildInsertSQL() const;
    String buildUpdateSQL() const;
    String buildDeleteSQL() const;

    // 获取参数
    const std::vector<MySQLParam>& params() const { return params_; }

    // 重置
    void reset();

private:
    enum class QueryType {
        SELECT,
        INSERT,
        UPDATE,
        DELETE_
    };

    QueryType type_;
    String table_;
    String columns_;
    String whereClause_;
    String orderByClause_;
    String limitClause_;
    String offsetClause_;
    HashMap<String, MySQLParam> data_;
    mutable std::vector<MySQLParam> params_;
};

} // namespace legend