/**
 * @file entity.cpp
 * @brief 实体基类实现
 */

#include "entity.h"
#include "database/mysql/mysql_connection.h"

namespace legend {

Entity::Entity(const String& tableName)
    : id_(0)
    , tableName_(tableName) {
}

bool Entity::isFieldDirty(const String& field) const {
    return dirtyFields_.find(field) != dirtyFields_.end();
}

void Entity::markDirty(const String& field) {
    dirtyFields_.insert(field);
}

void Entity::clearDirty() {
    dirtyFields_.clear();
}

bool Entity::save(MySQLConnection& conn) {
    if (isNew()) {
        String sql = getInsertSQL();
        std::vector<MySQLParam> params;
        bindInsertParams(params);
        int64 newId = conn.insertAndGetId(sql, params);
        if (newId > 0) {
            id_ = newId;
            clearDirty();
            return true;
        }
        return false;
    } else if (isDirty()) {
        String sql = getUpdateSQL();
        std::vector<MySQLParam> params;
        bindUpdateParams(params);
        int64 affected = conn.executeUpdate(sql, params);
        if (affected > 0) {
            clearDirty();
            return true;
        }
    }
    return false;
}

bool Entity::load(MySQLConnection& conn, int64 id) {
    // 子类实现
    return false;
}

bool Entity::remove(MySQLConnection& conn) {
    if (isNew()) return false;

    String sql = "DELETE FROM " + tableName_ + " WHERE id = ?";
    std::vector<MySQLParam> params = {MySQLParam(id_)};
    return conn.executeUpdate(sql, params) > 0;
}

// PlayerAccountEntity 实现
PlayerAccountEntity::PlayerAccountEntity()
    : Entity("player_account") {
}

String PlayerAccountEntity::getInsertSQL() const {
    return "INSERT INTO player_account (account_name, password_hash, salt, "
           "register_time, last_login_time, login_ip, status) VALUES (?, ?, ?, ?, ?, ?, ?)";
}

String PlayerAccountEntity::getUpdateSQL() const {
    String sql = "UPDATE player_account SET ";
    bool first = true;

    if (isFieldDirty("password_hash")) {
        sql += (first ? "" : ", ") + String("password_hash = ?");
        first = false;
    }
    if (isFieldDirty("last_login_time")) {
        sql += (first ? "" : ", ") + String("last_login_time = ?");
        first = false;
    }
    if (isFieldDirty("login_ip")) {
        sql += (first ? "" : ", ") + String("login_ip = ?");
        first = false;
    }
    if (isFieldDirty("status")) {
        sql += (first ? "" : ", ") + String("status = ?");
        first = false;
    }

    sql += " WHERE account_id = ?";
    return sql;
}

void PlayerAccountEntity::bindInsertParams(std::vector<MySQLParam>& params) const {
    params.push_back(MySQLParam(accountName_));
    params.push_back(MySQLParam(passwordHash_));
    params.push_back(MySQLParam(salt_));
    params.push_back(MySQLParam(registerTime_));
    params.push_back(MySQLParam(lastLoginTime_));
    params.push_back(MySQLParam(loginIp_));
    params.push_back(MySQLParam(static_cast<int64>(status_)));
}

void PlayerAccountEntity::bindUpdateParams(std::vector<MySQLParam>& params) const {
    if (isFieldDirty("password_hash")) {
        params.push_back(MySQLParam(passwordHash_));
    }
    if (isFieldDirty("last_login_time")) {
        params.push_back(MySQLParam(lastLoginTime_));
    }
    if (isFieldDirty("login_ip")) {
        params.push_back(MySQLParam(loginIp_));
    }
    if (isFieldDirty("status")) {
        params.push_back(MySQLParam(static_cast<int64>(status_)));
    }
    params.push_back(MySQLParam(id_));
}

// RoleEntity 实现
RoleEntity::RoleEntity()
    : Entity("player_role") {
}

String RoleEntity::getInsertSQL() const {
    return "INSERT INTO player_role (account_id, role_name, server_id, level, exp, "
           "profession, gender, hp, mp, gold, diamond) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
}

String RoleEntity::getUpdateSQL() const {
    String sql = "UPDATE player_role SET ";

    auto addField = [&](const String& name, bool dirty) {
        static bool first = true;
        if (dirty) {
            sql += (first ? "" : ", ") + name + " = ?";
            first = false;
        }
    };

    addField("level", isFieldDirty("level"));
    addField("exp", isFieldDirty("exp"));
    addField("hp", isFieldDirty("hp"));
    addField("mp", isFieldDirty("mp"));
    addField("gold", isFieldDirty("gold"));
    addField("diamond", isFieldDirty("diamond"));

    sql += " WHERE role_id = ?";
    return sql;
}

void RoleEntity::bindInsertParams(std::vector<MySQLParam>& params) const {
    params.push_back(MySQLParam(accountId_));
    params.push_back(MySQLParam(roleName_));
    params.push_back(MySQLParam(static_cast<int64>(serverId_)));
    params.push_back(MySQLParam(static_cast<int64>(level_)));
    params.push_back(MySQLParam(exp_));
    params.push_back(MySQLParam(static_cast<int64>(profession_)));
    params.push_back(MySQLParam(static_cast<int64>(gender_)));
    params.push_back(MySQLParam(static_cast<int64>(hp_)));
    params.push_back(MySQLParam(static_cast<int64>(mp_)));
    params.push_back(MySQLParam(gold_));
    params.push_back(MySQLParam(static_cast<int64>(diamond_)));
}

void RoleEntity::bindUpdateParams(std::vector<MySQLParam>& params) const {
    if (isFieldDirty("level")) {
        params.push_back(MySQLParam(static_cast<int64>(level_)));
    }
    if (isFieldDirty("exp")) {
        params.push_back(MySQLParam(exp_));
    }
    if (isFieldDirty("hp")) {
        params.push_back(MySQLParam(static_cast<int64>(hp_)));
    }
    if (isFieldDirty("mp")) {
        params.push_back(MySQLParam(static_cast<int64>(mp_)));
    }
    if (isFieldDirty("gold")) {
        params.push_back(MySQLParam(gold_));
    }
    if (isFieldDirty("diamond")) {
        params.push_back(MySQLParam(static_cast<int64>(diamond_)));
    }
    params.push_back(MySQLParam(id_));
}

} // namespace legend