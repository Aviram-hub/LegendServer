/**
 * @file chat_module.h
 * @brief 聊天模块
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "common/thread/mutex.h"
#include <unordered_map>

namespace legend {

class Session;

/**
 * @brief 聊天频道类型
 */
enum class ChatChannelType : int8 {
    WORLD = 0,
    PRIVATE = 1,
    TEAM = 2,
    GUILD = 3,
};

/**
 * @brief 聊天消息
 */
struct ChatMessage {
    ChatChannelType channel;
    int64 fromRoleId;
    String fromName;
    int64 toRoleId;
    String content;
    int64 timestamp;
};

/**
 * @brief 聊天模块
 */
class ChatModule : public NonCopyable {
public:
    ChatModule();
    ~ChatModule();

    void init();

    // 发送消息
    bool sendMessage(Session* session, ChatChannelType channel,
                     int64 toRoleId, const String& content);

    // 获取聊天记录
    Vector<ChatMessage> getHistory(ChatChannelType channel, int count);

private:
    bool checkCooldown(int64 roleId, ChatChannelType channel);

    mutable Mutex mutex_;
    HashMap<int64, int64> cooldowns_;  // roleId -> lastChatTime
    Vector<ChatMessage> worldMessages_;
};

} // namespace legend