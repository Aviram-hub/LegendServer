/**
 * @file scene.h
 * @brief 场景基类
 */

#pragma once

#include "types.h"
#include "noncopyable.h"
#include "aoi.h"
#include <unordered_map>

namespace legend {

/**
 * @brief 场景角色信息
 */
struct SceneRole {
    int64 roleId;
    int32 x;
    int32 y;
    int32 hp;
    int32 mp;
};

/**
 * @brief 场景类
 */
class Scene : public NonCopyable {
public:
    explicit Scene(int64 id, int32 width, int32 height);
    ~Scene();

    int64 id() const { return id_; }

    // 角色进入场景
    void enter(int64 roleId, int32 x, int32 y);

    // 角色离开场景
    void leave(int64 roleId);

    // 角色移动
    void move(int64 roleId, int32 newX, int32 newY);

    // 获取周围角色
    Vector<int64> getAroundRoles(int64 roleId);

    // 获取角色信息
    Ptr<SceneRole> getRole(int64 roleId);

    // 获取场景内角色数量
    size_t roleCount() const { return roles_.size(); }

private:
    int64 id_;
    int32 width_;
    int32 height_;

    UniquePtr<AOI> aoi_;
    HashMap<int64, Ptr<SceneRole>> roles_;
};

} // namespace legend