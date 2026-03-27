/**
 * @file entity.h
 * @brief 实体基类
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "database/mysql/mysql_connection.h"
#include <set>

namespace legend {

class MySQLConnection;

/**
 * @brief 实体基类
 */
class Entity : public NonCopyable {
public:
    Entity(const String& tableName);
    virtual ~Entity() = default;

    // 获取ID
    int64 id() const { return id_; }
    void setId(int64 id) { id_ = id; }

    // 表名
    const String& tableName() const { return tableName_; }

    // 是否新建
    bool isNew() const { return id_ == 0; }

    // 是否有修改
    bool isDirty() const { return !dirtyFields_.empty(); }
    bool isFieldDirty(const String& field) const;

    // 标记修改
    void markDirty(const String& field);
    void clearDirty();

    // 保存
    virtual bool save(MySQLConnection& conn);

    // 加载
    virtual bool load(MySQLConnection& conn, int64 id);

    // 删除
    virtual bool remove(MySQLConnection& conn);

protected:
    virtual String getInsertSQL() const = 0;
    virtual String getUpdateSQL() const = 0;
    virtual void bindInsertParams(std::vector<MySQLParam>& params) const = 0;
    virtual void bindUpdateParams(std::vector<MySQLParam>& params) const = 0;

    int64 id_;
    String tableName_;
    std::set<String> dirtyFields_;
};

/**
 * @brief 玩家账号实体
 */
class PlayerAccountEntity : public Entity {
public:
    PlayerAccountEntity();

    // 字段
    const String& accountName() const { return accountName_; }
    void setAccountName(const String& name) { accountName_ = name; markDirty("account_name"); }

    const String& passwordHash() const { return passwordHash_; }
    void setPasswordHash(const String& hash) { passwordHash_ = hash; markDirty("password_hash"); }

    const String& salt() const { return salt_; }
    void setSalt(const String& salt) { salt_ = salt; markDirty("salt"); }

    int64 registerTime() const { return registerTime_; }
    void setRegisterTime(int64 time) { registerTime_ = time; markDirty("register_time"); }

    int64 lastLoginTime() const { return lastLoginTime_; }
    void setLastLoginTime(int64 time) { lastLoginTime_ = time; markDirty("last_login_time"); }

    const String& loginIp() const { return loginIp_; }
    void setLoginIp(const String& ip) { loginIp_ = ip; markDirty("login_ip"); }

    int status() const { return status_; }
    void setStatus(int status) { status_ = status; markDirty("status"); }

private:
    String getInsertSQL() const override;
    String getUpdateSQL() const override;
    void bindInsertParams(std::vector<MySQLParam>& params) const override;
    void bindUpdateParams(std::vector<MySQLParam>& params) const override;

    String accountName_;
    String passwordHash_;
    String salt_;
    int64 registerTime_{0};
    int64 lastLoginTime_{0};
    String loginIp_;
    int status_{0};
};

/**
 * @brief 角色实体
 */
class RoleEntity : public Entity {
public:
    RoleEntity();

    // 字段
    int64 accountId() const { return accountId_; }
    void setAccountId(int64 id) { accountId_ = id; markDirty("account_id"); }

    const String& roleName() const { return roleName_; }
    void setRoleName(const String& name) { roleName_ = name; markDirty("role_name"); }

    int serverId() const { return serverId_; }
    void setServerId(int id) { serverId_ = id; markDirty("server_id"); }

    int level() const { return level_; }
    void setLevel(int level) { level_ = level; markDirty("level"); }

    int64 exp() const { return exp_; }
    void setExp(int64 exp) { exp_ = exp; markDirty("exp"); }

    int profession() const { return profession_; }
    void setProfession(int prof) { profession_ = prof; markDirty("profession"); }

    int gender() const { return gender_; }
    void setGender(int gender) { gender_ = gender; markDirty("gender"); }

    int hp() const { return hp_; }
    void setHp(int hp) { hp_ = hp; markDirty("hp"); }

    int mp() const { return mp_; }
    void setMp(int mp) { mp_ = mp; markDirty("mp"); }

    int64 gold() const { return gold_; }
    void setGold(int64 gold) { gold_ = gold; markDirty("gold"); }

    int diamond() const { return diamond_; }
    void setDiamond(int diamond) { diamond_ = diamond; markDirty("diamond"); }

private:
    String getInsertSQL() const override;
    String getUpdateSQL() const override;
    void bindInsertParams(std::vector<MySQLParam>& params) const override;
    void bindUpdateParams(std::vector<MySQLParam>& params) const override;

    int64 accountId_{0};
    String roleName_;
    int serverId_{1};
    int level_{1};
    int64 exp_{0};
    int profession_{0};
    int gender_{0};
    int hp_{100};
    int mp_{100};
    int64 gold_{0};
    int diamond_{0};
};

} // namespace legend