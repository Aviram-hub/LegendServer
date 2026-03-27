/**
 * @file scene.cpp
 * @brief 场景实现
 */

#include "scene.h"
#include "aoi.h"

namespace legend {

Scene::Scene(int64 id, int32 width, int32 height)
    : id_(id)
    , width_(width)
    , height_(height) {
    // 创建AOI系统
    int32 gridX = (width + 99) / 100;  // 每格100像素
    int32 gridY = (height + 99) / 100;
    aoi_ = std::make_unique<AOI>(width, height, gridX, gridY);
}

Scene::~Scene() = default;

void Scene::enter(int64 roleId, int32 x, int32 y) {
    auto role = std::make_shared<SceneRole>();
    role->roleId = roleId;
    role->x = x;
    role->y = y;

    roles_[roleId] = role;
    aoi_->enter(roleId, x, y);
}

void Scene::leave(int64 roleId) {
    roles_.erase(roleId);
    aoi_->leave(roleId);
}

void Scene::move(int64 roleId, int32 newX, int32 newY) {
    auto it = roles_.find(roleId);
    if (it != roles_.end()) {
        it->second->x = newX;
        it->second->y = newY;
        aoi_->move(roleId, newX, newY);
    }
}

Vector<int64> Scene::getAroundRoles(int64 roleId) {
    return aoi_->getAroundRoles(roleId);
}

Ptr<SceneRole> Scene::getRole(int64 roleId) {
    auto it = roles_.find(roleId);
    if (it != roles_.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace legend