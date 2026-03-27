/**
 * @file codec.h
 * @brief 编解码器
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include "message.h"
#include "common/base/buffer.h"

namespace legend {

/**
 * @brief 编解码器接口
 */
class Codec : public NonCopyable {
public:
    virtual ~Codec() = default;

    // 编码
    virtual bool encode(const Message& msg, Buffer& buffer) = 0;

    // 解码
    virtual Ptr<Message> decode(Buffer& buffer) = 0;

    // 获取消息长度
    virtual bool getMessageLength(Buffer& buffer, uint32& length) = 0;
};

/**
 * @brief 默认编解码器
 */
class DefaultCodec : public Codec {
public:
    DefaultCodec() = default;

    bool encode(const Message& msg, Buffer& buffer) override {
        // 先序列化消息体
        Buffer bodyBuffer;
        msg.serialize(bodyBuffer);

        // 构造消息头
        MessageHeader header;
        header.messageId = static_cast<uint32>(msg.messageId());
        header.length = HEADER_SIZE + bodyBuffer.readableBytes();

        // 写入头部
        header.serialize(buffer);

        // 写入消息体
        buffer.append(bodyBuffer.peek(), bodyBuffer.readableBytes());

        return true;
    }

    Ptr<Message> decode(Buffer& buffer) override {
        if (buffer.readableBytes() < HEADER_SIZE) {
            return nullptr;
        }

        // 解析头部
        MessageHeader header;
        size_t oldReadable = buffer.readableBytes();
        if (!header.deserialize(buffer)) {
            return nullptr;
        }

        // 检查是否有完整的消息
        if (buffer.readableBytes() < header.length - HEADER_SIZE) {
            // 还没有完整消息，恢复读指针
            buffer.retrieve(oldReadable - buffer.readableBytes());
            return nullptr;
        }

        // 创建消息
        Ptr<Message> msg = MessageFactory::instance().createMessage(
            static_cast<MessageId>(header.messageId));

        if (msg) {
            if (!msg->deserialize(buffer)) {
                return nullptr;
            }
        }

        return msg;
    }

    bool getMessageLength(Buffer& buffer, uint32& length) override {
        if (buffer.readableBytes() < HEADER_SIZE) {
            return false;
        }

        MessageHeader header;
        if (!header.deserialize(buffer)) {
            return false;
        }

        length = header.length;
        return true;
    }
};

/**
 * @brief 长度字段编解码器
 */
class LengthFieldCodec : public Codec {
public:
    LengthFieldCodec() = default;

    bool encode(const Message& msg, Buffer& buffer) override {
        Buffer bodyBuffer;
        msg.serialize(bodyBuffer);

        // 写入长度
        uint32 length = HEADER_SIZE + bodyBuffer.readableBytes();
        buffer.writeUInt32(length);

        // 写入消息ID
        buffer.writeUInt32(static_cast<uint32>(msg.messageId()));

        // 写入消息体
        buffer.append(bodyBuffer.peek(), bodyBuffer.readableBytes());

        return true;
    }

    Ptr<Message> decode(Buffer& buffer) override {
        if (buffer.readableBytes() < 8) {  // length + messageId
            return nullptr;
        }

        uint32 length = buffer.readUInt32();
        uint32 msgId = buffer.readUInt32();

        if (buffer.readableBytes() < length - 8) {
            return nullptr;
        }

        Ptr<Message> msg = MessageFactory::instance().createMessage(
            static_cast<MessageId>(msgId));

        if (msg && msg->deserialize(buffer)) {
            return msg;
        }

        return nullptr;
    }

    bool getMessageLength(Buffer& buffer, uint32& length) override {
        if (buffer.readableBytes() < 4) {
            return false;
        }

        length = buffer.peekInt32();
        return true;
    }
};

} // namespace legend