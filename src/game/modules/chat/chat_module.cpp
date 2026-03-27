/**
 * @file chat_module.cpp
 * @brief 聊天模块实现
 */

#include "chat_module.h"
#include "chat_channel.h"
#include "core/session/session.h"

namespace legend {

ChatModule::ChatModule() = default;
ChatModule::~ChatModule() = default;

void ChatModule::init() {
}

bool ChatModule::sendMessage(Session* session, ChatChannelType channel,
                              int64 toRoleId, const String& content) {
    if (!session) {
        return false;
    }

    // 检查冷却
    if (!checkCooldown(session->roleId(), channel)) {
        return false;
    }

    // 创建消息
    ChatMessage msg;
    msg.channel = channel;
    msg.fromRoleId = session->roleId();
    msg.toRoleId = toRoleId;
    msg.content = content;
    msg.timestamp = nowMs();

    // 根据频道类型处理
    switch (channel) {
        case ChatChannelType::WORLD:
            worldMessages_.push_back(msg);
            if (worldMessages_.size() > 100) {
                worldMessages_.erase(worldMessages_.begin());
            }
            break;
        case ChatChannelType::PRIVATE:
            // 私聊消息直接发送给目标
            break;
        case ChatChannelType::TEAM:
            // 发送给队伍成员
            break;
        case ChatChannelType::GUILD:
            // 发送给公会成员
            break;
    }

    return true;
}

Vector<ChatMessage> ChatModule::getHistory(ChatChannelType channel, int count) {
    Vector<ChatMessage> result;

    MutexGuard guard(mutex_);

    if (channel == ChatChannelType::WORLD) {
        size_t start = worldMessages_.size() > static_cast<size_t>(count)
                     ? worldMessages_.size() - count : 0;
        for (size_t i = start; i < worldMessages_.size(); ++i) {
            result.push_back(worldMessages_[i]);
        }
    }

    return result;
}

bool ChatModule::checkCooldown(int64 roleId, ChatChannelType channel) {
    MutexGuard guard(mutex_);

    int64 now = nowMs();
    int64 cd = 0;

    switch (channel) {
        case ChatChannelType::WORLD:
            cd = 5000;  // 5秒
            break;
        case ChatChannelType::PRIVATE:
            cd = 1000;  // 1秒
            break;
        default:
            cd = 0;
            break;
    }

    auto it = cooldowns_.find(roleId);
    if (it != cooldowns_.end()) {
        if (now - it->second < cd) {
            return false;
        }
    }

    cooldowns_[roleId] = now;
    return true;
}

} // namespace legend