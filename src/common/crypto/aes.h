/**
 * @file aes.h
 * @brief AES加密实现
 */

#pragma once

#include "common/base/types.h"
#include <array>

namespace legend {

/**
 * @brief AES加密类
 */
class AES {
public:
    static constexpr size_t kKeySize = 16;    // 128-bit key
    static constexpr size_t kBlockSize = 16;  // 128-bit block
    static constexpr size_t kIvSize = 16;     // 128-bit IV

    AES();
    ~AES();

    // 设置密钥
    bool setKey(const uint8* key, size_t keyLen);
    bool setKey(const String& key);

    // 设置IV
    void setIv(const uint8* iv, size_t ivLen);
    void setIv(const String& iv);

    // 加密
    String encrypt(const String& plaintext);
    String encrypt(const uint8* data, size_t len);

    // 解密
    String decrypt(const String& ciphertext);
    String decrypt(const uint8* data, size_t len);

    // CBC模式
    String encryptCBC(const String& plaintext);
    String decryptCBC(const String& ciphertext);

    // ECB模式
    String encryptECB(const String& plaintext);
    String decryptECB(const String& ciphertext);

    // 生成随机密钥
    static String generateKey();
    static String generateIv();

private:
    void keyExpansion(const uint8* key);
    void encryptBlock(uint8* block);
    void decryptBlock(uint8* block);

    std::array<uint32, 44> roundKeys_;
    std::array<uint8, kIvSize> iv_;
    bool keySet_;
};

/**
 * @brief AES工具函数
 */
namespace aes_util {

// PKCS7填充
String pkcs7Pad(const String& data, size_t blockSize);
String pkcs7Unpad(const String& data);

// XOR操作
void xorBlock(uint8* dst, const uint8* a, const uint8* b, size_t len);

} // namespace aes_util

} // namespace legend