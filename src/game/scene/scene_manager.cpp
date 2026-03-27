/**
 * @file scene_manager.cpp
 * @brief 场景管理器实现
 */

#include "scene_manager.h"
#include "scene.h"

namespace legend {

SceneManager& SceneManager::instance() {
    static SceneManager instance;
    return instance;
}

int64 SceneManager::createScene(int32 width, int32 height) {
    MutexGuard guard(mutex_);

    int64 sceneId = ++nextSceneId_;
    scenes_[sceneId] = std::make_shared<Scene>(sceneId, width, height);

    return sceneId;
}

void SceneManager::destroyScene(int64 sceneId) {
    MutexGuard guard(mutex_);
    scenes_.erase(sceneId);
}

Ptr<Scene> SceneManager::getScene(int64 sceneId) {
    MutexGuard guard(mutex_);

    auto it = scenes_.find(sceneId);
    if (it != scenes_.end()) {
        return it->second;
    }
    return nullptr;
}

int64 SceneManager::getRoleScene(int64 roleId) {
    MutexGuard guard(mutex_);

    auto it = roleScenes_.find(roleId);
    if (it != roleScenes_.end()) {
        return it->second;
    }
    return 0;
}

bool SceneManager::enterScene(int64 sceneId, int64 roleId, int32 x, int32 y) {
    MutexGuard guard(mutex_);

    auto it = scenes_.find(sceneId);
    if (it == scenes_.end()) {
        return false;
    }

    it->second->enter(roleId, x, y);
    roleScenes_[roleId] = sceneId;

    return true;
}

void SceneManager::leaveScene(int64 roleId) {
    MutexGuard guard(mutex_);

    auto sceneIt = roleScenes_.find(roleId);
    if (sceneIt != roleScenes_.end()) {
        auto scene = getScene(sceneIt->second);
        if (scene) {
            scene->leave(roleId);
        }
        roleScenes_.erase(sceneIt);
    }
}

} // namespace legend