/**
 * @file scene_manager.h
 * @brief 场景管理器
 */

#pragma once

#include "types.h"
#include "noncopyable.h"
#include "common/thread/mutex.h"

namespace legend {

class Scene;

/**
 * @brief 场景管理器
 */
class SceneManager : public NonCopyable {
public:
    static SceneManager& instance();

    // 创建场景
    int64 createScene(int32 width, int32 height);

    // 销毁场景
    void destroyScene(int64 sceneId);

    // 获取场景
    Ptr<Scene> getScene(int64 sceneId);

    // 获取角色所在场景
    int64 getRoleScene(int64 roleId);

    // 角色进入场景
    bool enterScene(int64 sceneId, int64 roleId, int32 x, int32 y);

    // 角色离开场景
    void leaveScene(int64 roleId);

private:
    SceneManager() = default;

    mutable Mutex mutex_;
    HashMap<int64, Ptr<Scene>> scenes_;
    HashMap<int64, int64> roleScenes_;  // roleId -> sceneId
    std::atomic<int64> nextSceneId_{0};
};

} // namespace legend