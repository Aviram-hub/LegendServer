/**
 * @file md5.h
 * @brief MD5加密实现
 */

#pragma once

#include "types.h"
#include <cstring>

namespace legend {

/**
 * @brief MD5加密类
 */
class MD5 {
public:
    MD5();
    explicit MD5(const String& data);
    MD5(const void* data, size_t len);

    // 更新数据
    void update(const void* data, size_t len);
    void update(const String& data);

    // 获取结果
    String hexDigest();
    String digest();

    // 静态方法
    static String hexDigest(const String& data);
    static String hexDigest(const void* data, size_t len);

private:
    void init();
    void transform(const uint8* block);
    void final();

    uint32 state_[4];
    uint32 count_[2];
    uint8 buffer_[64];
    uint8 digest_[16];
    bool finalized_;
};

/**
 * @brief MD5上下文
 */
struct MD5Context {
    uint32 state[4];
    uint32 count[2];
    uint8 buffer[64];
};

// MD5基本函数
inline uint32 F(uint32 x, uint32 y, uint32 z) {
    return (x & y) | (~x & z);
}

inline uint32 G(uint32 x, uint32 y, uint32 z) {
    return (x & z) | (y & ~z);
}

inline uint32 H(uint32 x, uint32 y, uint32 z) {
    return x ^ y ^ z;
}

inline uint32 I(uint32 x, uint32 y, uint32 z) {
    return y ^ (x | ~z);
}

// 循环左移
inline uint32 rotateLeft(uint32 x, int n) {
    return (x << n) | (x >> (32 - n));
}

// FF, GG, HH, II变换
inline void FF(uint32& a, uint32 b, uint32 c, uint32 d, uint32 x, uint32 s, uint32 ac) {
    a = rotateLeft(a + F(b, c, d) + x + ac, s) + b;
}

inline void GG(uint32& a, uint32 b, uint32 c, uint32 d, uint32 x, uint32 s, uint32 ac) {
    a = rotateLeft(a + G(b, c, d) + x + ac, s) + b;
}

inline void HH(uint32& a, uint32 b, uint32 c, uint32 d, uint32 x, uint32 s, uint32 ac) {
    a = rotateLeft(a + H(b, c, d) + x + ac, s) + b;
}

inline void II(uint32& a, uint32 b, uint32 c, uint32 d, uint32 x, uint32 s, uint32 ac) {
    a = rotateLeft(a + I(b, c, d) + x + ac, s) + b;
}

} // namespace legend