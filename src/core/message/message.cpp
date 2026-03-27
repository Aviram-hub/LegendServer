/**
 * @file message.cpp
 * @brief 消息实现
 */

#include "message.h"

namespace legend {

void MessageHeader::serialize(Buffer& buffer) const {
    buffer.writeUInt32(magic);
    buffer.append(&version, 1);
    buffer.writeUInt32(length);
    buffer.writeUInt32(messageId);
}

bool MessageHeader::deserialize(Buffer& buffer) {
    if (buffer.readableBytes() < HEADER_SIZE) {
        return false;
    }

    magic = buffer.readUInt32();
    buffer.readBytes(&version, 1);
    length = buffer.readUInt32();
    messageId = buffer.readUInt32();

    return magic == MAGIC_NUMBER && version == PROTOCOL_VERSION;
}

MessageFactory& MessageFactory::instance() {
    static MessageFactory factory;
    return factory;
}

void MessageFactory::registerMessage(MessageId id, MessageCreator creator) {
    creators_[id] = std::move(creator);
}

Ptr<Message> MessageFactory::createMessage(MessageId id) {
    auto it = creators_.find(id);
    if (it != creators_.end()) {
        return it->second();
    }
    return nullptr;
}

} // namespace legend