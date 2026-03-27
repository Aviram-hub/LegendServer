/**
 * @file role_module.h
 * @brief 角色模块
 */

#pragma once

#include "types.h"
#include "noncopyable.h"

namespace legend {

class Session;
class MySQLConnectionPool;
class RoleEntity;

/**
 * @brief 角色模块
 */
class RoleModule : public NonCopyable {
public:
    RoleModule();
    ~RoleModule();

    void init(MySQLConnectionPool* mysqlPool);

    // 创建角色
    int64 createRole(Session* session, const String& name, int profession, int gender);

    // 获取角色
    Ptr<RoleEntity> getRole(int64 roleId);

    // 获取账号下的角色列表
    Vector<int64> getRoleIdsByAccountId(int64 accountId);

    // 选择角色
    bool selectRole(Session* session, int64 roleId);

    // 更新角色属性
    void updateRole(Ptr<RoleEntity> role);

private:
    MySQLConnectionPool* mysqlPool_;
};

} // namespace legend