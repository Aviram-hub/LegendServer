/**
 * @file role_data.h
 * @brief 角色数据
 */

#pragma once

#include "types.h"
#include "database/orm/entity.h"

namespace legend {

/**
 * @brief 角色属性数据
 */
struct RoleAttributeData {
    int32 attack{10};
    int32 defense{10};
    int32 speed{10};
    float criticalRate{0.05f};
    float criticalDamage{1.5f};
    int32 hpMax{100};
    int32 mpMax{100};
};

/**
 * @brief 角色完整数据
 */
struct RoleData {
    int64 roleId{0};
    int64 accountId{0};
    String name;
    int32 level{1};
    int64 exp{0};
    int32 profession{0};
    int32 gender{0};
    int32 hp{100};
    int32 mp{100};
    int64 gold{0};
    int32 diamond{0};

    RoleAttributeData attributes;
};

} // namespace legend