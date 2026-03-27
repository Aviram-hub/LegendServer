/**
 * @file md5.cpp
 * @brief MD5实现
 */

#include "md5.h"

namespace legend {

MD5::MD5()
    : finalized_(false) {
    init();
}

MD5::MD5(const String& data)
    : finalized_(false) {
    init();
    update(data);
}

MD5::MD5(const void* data, size_t len)
    : finalized_(false) {
    init();
    update(data, len);
}

void MD5::init() {
    state_[0] = 0x67452301;
    state_[1] = 0xefcdab89;
    state_[2] = 0x98badcfe;
    state_[3] = 0x10325476;

    count_[0] = 0;
    count_[1] = 0;
    finalized_ = false;
}

void MD5::update(const void* data, size_t len) {
    if (finalized_) {
        return;
    }

    const uint8* input = static_cast<const uint8*>(data);
    size_t index = (count_[0] >> 3) & 0x3F;
    size_t partLen = 64 - index;

    count_[0] += (len << 3);
    if (count_[0] < (len << 3)) {
        count_[1]++;
    }
    count_[1] += (len >> 29);

    size_t i = 0;
    if (len >= partLen) {
        memcpy(&buffer_[index], input, partLen);
        transform(buffer_);

        for (i = partLen; i + 63 < len; i += 64) {
            transform(&input[i]);
        }

        index = 0;
    }

    memcpy(&buffer_[index], &input[i], len - i);
}

void MD5::update(const String& data) {
    update(data.data(), data.size());
}

void MD5::transform(const uint8* block) {
    uint32 a = state_[0], b = state_[1], c = state_[2], d = state_[3];

    const uint32* x = reinterpret_cast<const uint32*>(block);

    // Round 1
    FF(a, b, c, d, x[0], 7, 0xd76aa478);
    FF(d, a, b, c, x[1], 12, 0xe8c7b756);
    FF(c, d, a, b, x[2], 17, 0x242070db);
    FF(b, c, d, a, x[3], 22, 0xc1bdceee);
    FF(a, b, c, d, x[4], 7, 0xf57c0faf);
    FF(d, a, b, c, x[5], 12, 0x4787c62a);
    FF(c, d, a, b, x[6], 17, 0xa8304613);
    FF(b, c, d, a, x[7], 22, 0xfd469501);
    FF(a, b, c, d, x[8], 7, 0x698098d8);
    FF(d, a, b, c, x[9], 12, 0x8b44f7af);
    FF(c, d, a, b, x[10], 17, 0xffff5bb1);
    FF(b, c, d, a, x[11], 22, 0x895cd7be);
    FF(a, b, c, d, x[12], 7, 0x6b901122);
    FF(d, a, b, c, x[13], 12, 0xfd987193);
    FF(c, d, a, b, x[14], 17, 0xa679438e);
    FF(b, c, d, a, x[15], 22, 0x49b40821);

    // Round 2
    GG(a, b, c, d, x[1], 5, 0xf61e2562);
    GG(d, a, b, c, x[6], 9, 0xc040b340);
    GG(c, d, a, b, x[11], 14, 0x265e5a51);
    GG(b, c, d, a, x[0], 20, 0xe9b6c7aa);
    GG(a, b, c, d, x[5], 5, 0xd62f105d);
    GG(d, a, b, c, x[10], 9, 0x02441453);
    GG(c, d, a, b, x[15], 14, 0xd8a1e681);
    GG(b, c, d, a, x[4], 20, 0xe7d3fbc8);
    GG(a, b, c, d, x[9], 5, 0x21e1cde6);
    GG(d, a, b, c, x[14], 9, 0xc33707d6);
    GG(c, d, a, b, x[3], 14, 0xf4d50d87);
    GG(b, c, d, a, x[8], 20, 0x455a14ed);
    GG(a, b, c, d, x[13], 5, 0xa9e3e905);
    GG(d, a, b, c, x[2], 9, 0xfcefa3f8);
    GG(c, d, a, b, x[7], 14, 0x676f02d9);
    GG(b, c, d, a, x[12], 20, 0x8d2a4c8a);

    // Round 3
    HH(a, b, c, d, x[5], 4, 0xfffa3942);
    HH(d, a, b, c, x[8], 11, 0x8771f681);
    HH(c, d, a, b, x[11], 16, 0x6d9d6122);
    HH(b, c, d, a, x[14], 23, 0xfde5380c);
    HH(a, b, c, d, x[1], 4, 0xa4beea44);
    HH(d, a, b, c, x[4], 11, 0x4bdecfa9);
    HH(c, d, a, b, x[7], 16, 0xf6bb4b60);
    HH(b, c, d, a, x[10], 23, 0xbebfbc70);
    HH(a, b, c, d, x[13], 4, 0x289b7ec6);
    HH(d, a, b, c, x[0], 11, 0xeaa127fa);
    HH(c, d, a, b, x[3], 16, 0xd4ef3085);
    HH(b, c, d, a, x[6], 23, 0x04881d05);
    HH(a, b, c, d, x[9], 4, 0xd9d4d039);
    HH(d, a, b, c, x[12], 11, 0xe6db99e5);
    HH(c, d, a, b, x[15], 16, 0x1fa27cf8);
    HH(b, c, d, a, x[2], 23, 0xc4ac5665);

    // Round 4
    II(a, b, c, d, x[0], 6, 0xf4292244);
    II(d, a, b, c, x[7], 10, 0x432aff97);
    II(c, d, a, b, x[14], 15, 0xab9423a7);
    II(b, c, d, a, x[5], 21, 0xfc93a039);
    II(a, b, c, d, x[12], 6, 0x655b59c3);
    II(d, a, b, c, x[3], 10, 0x8f0ccc92);
    II(c, d, a, b, x[10], 15, 0xffeff47d);
    II(b, c, d, a, x[1], 21, 0x85845dd1);
    II(a, b, c, d, x[8], 6, 0x6fa87e4f);
    II(d, a, b, c, x[15], 10, 0xfe2ce6e0);
    II(c, d, a, b, x[6], 15, 0xa3014314);
    II(b, c, d, a, x[13], 21, 0x4e0811a1);
    II(a, b, c, d, x[4], 6, 0xf7537e82);
    II(d, a, b, c, x[11], 10, 0xbd3af235);
    II(c, d, a, b, x[2], 15, 0x2ad7d2bb);
    II(b, c, d, a, x[9], 21, 0xeb86d391);

    state_[0] += a;
    state_[1] += b;
    state_[2] += c;
    state_[3] += d;
}

void MD5::final() {
    static uint8 padding[64] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    uint8 bits[8];
    memcpy(bits, count_, 8);

    size_t index = (count_[0] >> 3) & 0x3f;
    size_t padLen = (index < 56) ? (56 - index) : (120 - index);
    update(padding, padLen);
    update(bits, 8);

    for (size_t i = 0; i < 16; i++) {
        digest_[i] = static_cast<uint8>((state_[i >> 2] >> ((i % 4) * 8)) & 0xff);
    }

    finalized_ = true;
}

String MD5::hexDigest() {
    if (!finalized_) {
        final();
    }

    static const char hex[] = "0123456789abcdef";
    String result;
    result.reserve(32);

    for (size_t i = 0; i < 16; i++) {
        result.push_back(hex[(digest_[i] >> 4) & 0x0f]);
        result.push_back(hex[digest_[i] & 0x0f]);
    }

    return result;
}

String MD5::digest() {
    if (!finalized_) {
        final();
    }
    return String(reinterpret_cast<char*>(digest_), 16);
}

String MD5::hexDigest(const String& data) {
    MD5 md5(data);
    return md5.hexDigest();
}

String MD5::hexDigest(const void* data, size_t len) {
    MD5 md5(data, len);
    return md5.hexDigest();
}

} // namespace legend