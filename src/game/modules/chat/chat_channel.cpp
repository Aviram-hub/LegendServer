/**
 * @file chat_channel.cpp
 * @brief 聊天频道实现
 */

#include "chat_channel.h"

namespace legend {

ChatChannel::ChatChannel(ChatChannelType type, size_t maxMessages)
    : type_(type)
    , maxMessages_(maxMessages) {
}

void ChatChannel::addMessage(const ChatMessage& msg) {
    messages_.push_back(msg);

    // 保持消息数量限制
    while (messages_.size() > maxMessages_) {
        messages_.pop_front();
    }
}

Vector<ChatMessage> ChatChannel::getMessages(size_t count) const {
    Vector<ChatMessage> result;

    size_t start = messages_.size() > count ? messages_.size() - count : 0;
    auto it = messages_.begin();
    std::advance(it, start);

    for (; it != messages_.end(); ++it) {
        result.push_back(*it);
    }

    return result;
}

void ChatChannel::clear() {
    messages_.clear();
}

} // namespace legend