/**
 * @file friend_manager.h
 * @brief 好友管理器
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "common/thread/mutex.h"
#include <unordered_map>
#include <unordered_set>

namespace legend {

/**
 * @brief 好友管理器
 */
class FriendManager : public NonCopyable {
public:
    static FriendManager& instance();

    // 添加好友关系
    void addFriend(int64 roleId, int64 friendId);

    // 移除好友关系
    void removeFriend(int64 roleId, int64 friendId);

    // 检查好友关系
    bool isFriend(int64 roleId, int64 friendId);

    // 获取好友列表
    const HashSet<int64>& getFriends(int64 roleId);

private:
    FriendManager() = default;

    mutable Mutex mutex_;
    HashMap<int64, HashSet<int64>> friendMap_;  // roleId -> set of friendIds
};

} // namespace legend