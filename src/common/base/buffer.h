/**
 * @file buffer.h
 * @brief 高性能缓冲区类
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include <cstring>
#include <algorithm>

namespace legend {

/**
 * @brief 缓冲区类
 * 用于网络数据收发的高效缓冲区实现
 */
class Buffer : public NonCopyable {
public:
    static constexpr size_t kInitialSize = 1024;
    static constexpr size_t kCheapPrepend = 8;

    Buffer()
        : buffer_(kCheapPrepend + kInitialSize)
        , readerIndex_(kCheapPrepend)
        , writerIndex_(kCheapPrepend) {
    }

    explicit Buffer(size_t initialSize)
        : buffer_(kCheapPrepend + initialSize)
        , readerIndex_(kCheapPrepend)
        , writerIndex_(kCheapPrepend) {
    }

    Buffer(Buffer&& other) noexcept
        : buffer_(std::move(other.buffer_))
        , readerIndex_(other.readerIndex_)
        , writerIndex_(other.writerIndex_) {
        other.readerIndex_ = kCheapPrepend;
        other.writerIndex_ = kCheapPrepend;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            buffer_ = std::move(other.buffer_);
            readerIndex_ = other.readerIndex_;
            writerIndex_ = other.writerIndex_;
            other.readerIndex_ = kCheapPrepend;
            other.writerIndex_ = kCheapPrepend;
        }
        return *this;
    }

    // 可读区域大小
    size_t readableBytes() const {
        return writerIndex_ - readerIndex_;
    }

    // 可写区域大小
    size_t writableBytes() const {
        return buffer_.size() - writerIndex_;
    }

    // 前置预留区域大小
    size_t prependableBytes() const {
        return readerIndex_;
    }

    // 获取读指针
    const char* peek() const {
        return begin() + readerIndex_;
    }

    // 获取写指针
    char* beginWrite() {
        return begin() + writerIndex_;
    }

    const char* beginWrite() const {
        return begin() + writerIndex_;
    }

    // 查找特定字符
    const char* findCRLF() const {
        const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
        return crlf == beginWrite() ? nullptr : crlf;
    }

    // 跳过n字节
    void retrieve(size_t len) {
        if (len < readableBytes()) {
            readerIndex_ += len;
        } else {
            retrieveAll();
        }
    }

    // 跳过所有
    void retrieveAll() {
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }

    // 读取所有数据为字符串
    String retrieveAllAsString() {
        return retrieveAsString(readableBytes());
    }

    // 读取n字节为字符串
    String retrieveAsString(size_t len) {
        String result(peek(), len);
        retrieve(len);
        return result;
    }

    // 写入数据
    void append(const char* data, size_t len) {
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        hasWritten(len);
    }

    void append(const void* data, size_t len) {
        append(static_cast<const char*>(data), len);
    }

    void append(const String& str) {
        append(str.data(), str.size());
    }

    // 确保可写空间
    void ensureWritableBytes(size_t len) {
        if (writableBytes() < len) {
            makeSpace(len);
        }
    }

    // 已写入n字节
    void hasWritten(size_t len) {
        writerIndex_ += len;
    }

    // 预留数据
    void prepend(const void* data, size_t len) {
        readerIndex_ -= len;
        const char* d = static_cast<const char*>(data);
        std::copy(d, d + len, begin() + readerIndex_);
    }

    // 读取int32
    int32 readInt32() {
        int32 result = 0;
        ::memcpy(&result, peek(), sizeof(result));
        result = ntoh32(result);
        retrieve(sizeof(result));
        return result;
    }

    // 读取字节到缓冲区
    void readBytes(void* dest, size_t len) {
        ::memcpy(dest, peek(), len);
        retrieve(len);
    }

    // 读取uint32
    uint32 readUInt32() {
        uint32 result = 0;
        ::memcpy(&result, peek(), sizeof(result));
        result = ntoh32(result);
        retrieve(sizeof(result));
        return result;
    }

    // 写入int32
    void writeInt32(int32 value) {
        int32 v = hton32(value);
        append(&v, sizeof(v));
    }

    // 写入uint32
    void writeUInt32(uint32 value) {
        uint32 v = hton32(value);
        append(&v, sizeof(v));
    }

    // 查看int32（不移动读指针）
    int32 peekInt32() const {
        int32 result = 0;
        ::memcpy(&result, peek(), sizeof(result));
        return ntoh32(result);
    }

    // 缩容
    void shrink(size_t reserve) {
        Buffer other;
        other.ensureWritableBytes(readableBytes() + reserve);
        other.append(peek(), readableBytes());
        swap(other);
    }

    // 交换
    void swap(Buffer& other) {
        buffer_.swap(other.buffer_);
        std::swap(readerIndex_, other.readerIndex_);
        std::swap(writerIndex_, other.writerIndex_);
    }

    // 容量
    size_t capacity() const {
        return buffer_.size();
    }

    // 内部缓冲区指针
    const char* data() const {
        return buffer_.data();
    }

    // 从文件描述符读取数据
    ssize_t readFd(int fd, int* savedErrno);

private:
    char* begin() {
        return buffer_.data();
    }

    const char* begin() const {
        return buffer_.data();
    }

    void makeSpace(size_t len) {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
            buffer_.resize(writerIndex_ + len);
        } else {
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_,
                      begin() + writerIndex_,
                      begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
        }
    }

    // 网络字节序转换
    static uint32 hton32(uint32 v) {
        return (v >> 24) | ((v >> 8) & 0xFF00) | ((v << 8) & 0xFF0000) | (v << 24);
    }

    static uint32 ntoh32(uint32 v) {
        return hton32(v);
    }

    static constexpr char kCRLF[] = "\r\n";

    Vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};

} // namespace legend