/**
 * @file message.h
 * @brief 消息基类
 */

#pragma once

#include "common/base/types.h"
#include "common/base/buffer.h"

namespace legend {

/**
 * @brief 消息基类
 */
class Message {
public:
    Message() = default;
    virtual ~Message() = default;

    // 获取消息ID
    virtual MessageId messageId() const = 0;

    // 序列化
    virtual void serialize(Buffer& buffer) const = 0;

    // 反序列化
    virtual bool deserialize(Buffer& buffer) = 0;

    // 获取消息大小
    virtual size_t size() const = 0;
};

/**
 * @brief 消息头
 */
struct MessageHeader {
    uint32 magic;       // 魔数
    uint8 version;      // 版本
    uint32 length;      // 长度
    uint32 messageId;   // 消息ID

    MessageHeader()
        : magic(MAGIC_NUMBER)
        , version(PROTOCOL_VERSION)
        , length(0)
        , messageId(0) {
    }

    void serialize(Buffer& buffer) const;
    bool deserialize(Buffer& buffer);
};

/**
 * @brief 消息工厂
 */
class MessageFactory {
public:
    using MessageCreator = std::function<Ptr<Message>()>;

    static MessageFactory& instance();

    void registerMessage(MessageId id, MessageCreator creator);
    Ptr<Message> createMessage(MessageId id);

private:
    MessageFactory() = default;
    HashMap<MessageId, MessageCreator> creators_;
};

/**
 * @brief 消息注册器
 */
template<typename T>
class MessageRegistrar {
public:
    explicit MessageRegistrar(MessageId id) {
        MessageFactory::instance().registerMessage(id, []() {
            return std::make_shared<T>();
        });
    }
};

// 注册消息的宏
#define REGISTER_MESSAGE(id, cls) \
    static legend::MessageRegistrar<cls> _registrar_##cls(id)

} // namespace legend