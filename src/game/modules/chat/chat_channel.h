/**
 * @file chat_channel.h
 * @brief 聊天频道
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "chat_module.h"
#include <list>

namespace legend {

/**
 * @brief 聊天频道
 */
class ChatChannel : public NonCopyable {
public:
    explicit ChatChannel(ChatChannelType type, size_t maxMessages = 100);

    // 添加消息
    void addMessage(const ChatMessage& msg);

    // 获取消息列表
    Vector<ChatMessage> getMessages(size_t count) const;

    // 清空消息
    void clear();

    ChatChannelType type() const { return type_; }
    size_t messageCount() const { return messages_.size(); }

private:
    ChatChannelType type_;
    size_t maxMessages_;
    std::list<ChatMessage> messages_;
};

} // namespace legend