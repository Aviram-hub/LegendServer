/**
 * @file buffer_stream.h
 * @brief 缓冲流封装
 */

#pragma once

#include "common/base/types.h"
#include "common/base/buffer.h"
#include <sstream>

namespace legend {

/**
 * @brief 输入缓冲流
 */
class BufferInputStream {
public:
    explicit BufferInputStream(Buffer& buffer)
        : buffer_(buffer) {
    }

    // 读取整数
    int8 readInt8() {
        int8 value;
        buffer_.retrieve(buffer_.readableBytes() >= sizeof(value) ? sizeof(value) : 0);
        memcpy(&value, buffer_.peek(), sizeof(value));
        buffer_.retrieve(sizeof(value));
        return value;
    }

    int16 readInt16() {
        int16 value;
        memcpy(&value, buffer_.peek(), sizeof(value));
        value = static_cast<int16>(ntoh16(static_cast<uint16>(value)));
        buffer_.retrieve(sizeof(value));
        return value;
    }

    int32 readInt32() {
        return buffer_.readInt32();
    }

    int64 readInt64() {
        int64 value;
        memcpy(&value, buffer_.peek(), sizeof(value));
        value = static_cast<int64>(ntoh64(static_cast<uint64>(value)));
        buffer_.retrieve(sizeof(value));
        return value;
    }

    uint8 readUInt8() {
        uint8 value;
        memcpy(&value, buffer_.peek(), sizeof(value));
        buffer_.retrieve(sizeof(value));
        return value;
    }

    uint16 readUInt16() {
        uint16 value;
        memcpy(&value, buffer_.peek(), sizeof(value));
        value = ntoh16(value);
        buffer_.retrieve(sizeof(value));
        return value;
    }

    uint32 readUInt32() {
        return buffer_.readUInt32();
    }

    uint64 readUInt64() {
        uint64 value;
        memcpy(&value, buffer_.peek(), sizeof(value));
        value = ntoh64(value);
        buffer_.retrieve(sizeof(value));
        return value;
    }

    float readFloat() {
        uint32 value = readUInt32();
        return *reinterpret_cast<float*>(&value);
    }

    double readDouble() {
        uint64 value = readUInt64();
        return *reinterpret_cast<double*>(&value);
    }

    String readString() {
        uint16 len = readUInt16();
        String str(buffer_.peek(), len);
        buffer_.retrieve(len);
        return str;
    }

    String readStringWithLen(uint32 len) {
        String str(buffer_.peek(), len);
        buffer_.retrieve(len);
        return str;
    }

    void readBytes(void* data, size_t len) {
        memcpy(data, buffer_.peek(), len);
        buffer_.retrieve(len);
    }

    size_t readableBytes() const {
        return buffer_.readableBytes();
    }

private:
    Buffer& buffer_;

    static uint16 ntoh16(uint16 v) {
        return (v >> 8) | (v << 8);
    }

    static uint64 ntoh64(uint64 v) {
        return ((v >> 56) & 0x00000000000000FF) |
               ((v >> 40) & 0x000000000000FF00) |
               ((v >> 24) & 0x0000000000FF0000) |
               ((v >> 8)  & 0x00000000FF000000) |
               ((v << 8)  & 0x000000FF00000000) |
               ((v << 24) & 0x0000FF0000000000) |
               ((v << 40) & 0x00FF000000000000) |
               ((v << 56) & 0xFF00000000000000);
    }
};

/**
 * @brief 输出缓冲流
 */
class BufferOutputStream {
public:
    explicit BufferOutputStream(Buffer& buffer)
        : buffer_(buffer) {
    }

    void writeInt8(int8 value) {
        buffer_.append(&value, sizeof(value));
    }

    void writeInt16(int16 value) {
        uint16 v = hton16(static_cast<uint16>(value));
        buffer_.append(&v, sizeof(v));
    }

    void writeInt32(int32 value) {
        buffer_.writeInt32(value);
    }

    void writeInt64(int64 value) {
        uint64 v = hton64(static_cast<uint64>(value));
        buffer_.append(&v, sizeof(v));
    }

    void writeUInt8(uint8 value) {
        buffer_.append(&value, sizeof(value));
    }

    void writeUInt16(uint16 value) {
        uint16 v = hton16(value);
        buffer_.append(&v, sizeof(v));
    }

    void writeUInt32(uint32 value) {
        buffer_.writeUInt32(value);
    }

    void writeUInt64(uint64 value) {
        uint64 v = hton64(value);
        buffer_.append(&v, sizeof(v));
    }

    void writeFloat(float value) {
        uint32 v = *reinterpret_cast<uint32*>(&value);
        writeUInt32(v);
    }

    void writeDouble(double value) {
        uint64 v = *reinterpret_cast<uint64*>(&value);
        writeUInt64(v);
    }

    void writeString(const String& value) {
        writeUInt16(static_cast<uint16>(value.size()));
        buffer_.append(value);
    }

    void writeString(const char* data, size_t len) {
        writeUInt16(static_cast<uint16>(len));
        buffer_.append(data, len);
    }

    void writeBytes(const void* data, size_t len) {
        buffer_.append(data, len);
    }

    size_t writableBytes() const {
        return buffer_.writableBytes();
    }

private:
    Buffer& buffer_;

    static uint16 hton16(uint16 v) {
        return (v >> 8) | (v << 8);
    }

    static uint64 hton64(uint64 v) {
        return ((v >> 56) & 0x00000000000000FF) |
               ((v >> 40) & 0x000000000000FF00) |
               ((v >> 24) & 0x0000000000FF0000) |
               ((v >> 8)  & 0x00000000FF000000) |
               ((v << 8)  & 0x000000FF00000000) |
               ((v << 24) & 0x0000FF0000000000) |
               ((v << 40) & 0x00FF000000000000) |
               ((v << 56) & 0xFF00000000000000);
    }
};

} // namespace legend