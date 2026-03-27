/**
 * @file friend_manager.cpp
 * @brief 好友管理器实现
 */

#include "friend_manager.h"

namespace legend {

FriendManager& FriendManager::instance() {
    static FriendManager instance;
    return instance;
}

void FriendManager::addFriend(int64 roleId, int64 friendId) {
    MutexGuard guard(mutex_);
    friendMap_[roleId].insert(friendId);
    friendMap_[friendId].insert(roleId);
}

void FriendManager::removeFriend(int64 roleId, int64 friendId) {
    MutexGuard guard(mutex_);

    auto it = friendMap_.find(roleId);
    if (it != friendMap_.end()) {
        it->second.erase(friendId);
    }

    it = friendMap_.find(friendId);
    if (it != friendMap_.end()) {
        it->second.erase(roleId);
    }
}

bool FriendManager::isFriend(int64 roleId, int64 friendId) {
    MutexGuard guard(mutex_);

    auto it = friendMap_.find(roleId);
    if (it != friendMap_.end()) {
        return it->second.find(friendId) != it->second.end();
    }
    return false;
}

const HashSet<int64>& FriendManager::getFriends(int64 roleId) {
    static const HashSet<int64> emptySet;

    MutexGuard guard(mutex_);

    auto it = friendMap_.find(roleId);
    if (it != friendMap_.end()) {
        return it->second;
    }
    return emptySet;
}

} // namespace legend