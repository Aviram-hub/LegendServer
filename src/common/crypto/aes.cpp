/**
 * @file aes.cpp
 * @brief AES实现
 */

#include "aes.h"
#include "random.h"
#include <cstring>

namespace legend {

// AES S-box
static const uint8 sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// Inverse S-box
static const uint8 inv_sbox[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

// Round constants
static const uint8 rcon[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

AES::AES()
    : keySet_(false) {
}

AES::~AES() = default;

bool AES::setKey(const uint8* key, size_t keyLen) {
    if (keyLen != kKeySize) {
        return false;
    }
    keyExpansion(key);
    keySet_ = true;
    return true;
}

bool AES::setKey(const String& key) {
    return setKey(reinterpret_cast<const uint8*>(key.data()), key.size());
}

void AES::setIv(const uint8* iv, size_t ivLen) {
    if (ivLen >= kIvSize) {
        memcpy(iv_.data(), iv, kIvSize);
    }
}

void AES::setIv(const String& iv) {
    setIv(reinterpret_cast<const uint8*>(iv.data()), iv.size());
}

void AES::keyExpansion(const uint8* key) {
    for (int i = 0; i < 4; i++) {
        roundKeys_[i] = (key[4*i] << 24) | (key[4*i+1] << 16) |
                        (key[4*i+2] << 8) | key[4*i+3];
    }

    for (int i = 4; i < 44; i++) {
        uint32 temp = roundKeys_[i-1];
        if (i % 4 == 0) {
            // RotWord + SubWord + Rcon
            temp = (sbox[(temp >> 16) & 0xff] << 24) |
                   (sbox[(temp >> 8) & 0xff] << 16) |
                   (sbox[temp & 0xff] << 8) |
                   sbox[(temp >> 24) & 0xff];
            temp ^= rcon[i/4] << 24;
        }
        roundKeys_[i] = roundKeys_[i-4] ^ temp;
    }
}

String AES::encrypt(const String& plaintext) {
    return encryptCBC(plaintext);
}

String AES::decrypt(const String& ciphertext) {
    return decryptCBC(ciphertext);
}

String AES::encryptCBC(const String& plaintext) {
    if (!keySet_) return "";

    String padded = aes_util::pkcs7Pad(plaintext, kBlockSize);
    String result;
    result.reserve(padded.size());

    uint8 block[kBlockSize];
    uint8 iv[kIvSize];
    memcpy(iv, iv_.data(), kIvSize);

    for (size_t i = 0; i < padded.size(); i += kBlockSize) {
        aes_util::xorBlock(block, reinterpret_cast<const uint8*>(&padded[i]), iv, kBlockSize);
        encryptBlock(block);
        memcpy(iv, block, kBlockSize);
        result.append(reinterpret_cast<char*>(block), kBlockSize);
    }

    return result;
}

String AES::decryptCBC(const String& ciphertext) {
    if (!keySet_ || ciphertext.size() % kBlockSize != 0) return "";

    String result;
    result.reserve(ciphertext.size());

    uint8 block[kBlockSize];
    uint8 prev[kIvSize];
    memcpy(prev, iv_.data(), kIvSize);

    for (size_t i = 0; i < ciphertext.size(); i += kBlockSize) {
        memcpy(block, &ciphertext[i], kBlockSize);
        decryptBlock(block);

        uint8 temp[kBlockSize];
        aes_util::xorBlock(temp, block, prev, kBlockSize);
        memcpy(prev, &ciphertext[i], kBlockSize);

        result.append(reinterpret_cast<char*>(temp), kBlockSize);
    }

    return aes_util::pkcs7Unpad(result);
}

void AES::encryptBlock(uint8* block) {
    // AddRoundKey
    for (int i = 0; i < 16; i += 4) {
        uint32 k = roundKeys_[i/4];
        block[i] ^= (k >> 24) & 0xff;
        block[i+1] ^= (k >> 16) & 0xff;
        block[i+2] ^= (k >> 8) & 0xff;
        block[i+3] ^= k & 0xff;
    }

    // 9 rounds
    for (int round = 1; round < 10; round++) {
        // SubBytes
        for (int i = 0; i < 16; i++) {
            block[i] = sbox[block[i]];
        }

        // ShiftRows
        // Row 1: shift left 1
        uint8 t = block[1]; block[1] = block[5]; block[5] = block[9]; block[9] = block[13]; block[13] = t;
        // Row 2: shift left 2
        t = block[2]; block[2] = block[10]; block[10] = t;
        t = block[6]; block[6] = block[14]; block[14] = t;
        // Row 3: shift left 3
        t = block[15]; block[15] = block[11]; block[11] = block[7]; block[7] = block[3]; block[3] = t;

        // MixColumns
        for (int i = 0; i < 16; i += 4) {
            uint8 a[4] = {block[i], block[i+1], block[i+2], block[i+3]};
            block[i] = a[0] ^ a[1] ^ a[2] ^ a[3] ^ a[1] ^ a[2];
            block[i+1] = a[0] ^ a[1] ^ a[2] ^ a[3] ^ a[0] ^ a[2];
            block[i+2] = a[0] ^ a[1] ^ a[2] ^ a[3] ^ a[0] ^ a[1];
            block[i+3] = a[0] ^ a[1] ^ a[2] ^ a[3] ^ a[1] ^ a[3];
        }

        // AddRoundKey
        for (int i = 0; i < 16; i += 4) {
            uint32 k = roundKeys_[round * 4 + i/4];
            block[i] ^= (k >> 24) & 0xff;
            block[i+1] ^= (k >> 16) & 0xff;
            block[i+2] ^= (k >> 8) & 0xff;
            block[i+3] ^= k & 0xff;
        }
    }

    // Final round
    for (int i = 0; i < 16; i++) {
        block[i] = sbox[block[i]];
    }

    uint8 t = block[1]; block[1] = block[5]; block[5] = block[9]; block[9] = block[13]; block[13] = t;
    t = block[2]; block[2] = block[10]; block[10] = t;
    t = block[6]; block[6] = block[14]; block[14] = t;
    t = block[15]; block[15] = block[11]; block[11] = block[7]; block[7] = block[3]; block[3] = t;

    for (int i = 0; i < 16; i += 4) {
        uint32 k = roundKeys_[40 + i/4];
        block[i] ^= (k >> 24) & 0xff;
        block[i+1] ^= (k >> 16) & 0xff;
        block[i+2] ^= (k >> 8) & 0xff;
        block[i+3] ^= k & 0xff;
    }
}

void AES::decryptBlock(uint8* block) {
    // Initial AddRoundKey
    for (int i = 0; i < 16; i += 4) {
        uint32 k = roundKeys_[40 + i/4];
        block[i] ^= (k >> 24) & 0xff;
        block[i+1] ^= (k >> 16) & 0xff;
        block[i+2] ^= (k >> 8) & 0xff;
        block[i+3] ^= k & 0xff;
    }

    // 9 rounds in reverse
    for (int round = 9; round >= 1; round--) {
        // InvShiftRows
        uint8 t = block[13]; block[13] = block[9]; block[9] = block[5]; block[5] = block[1]; block[1] = t;
        t = block[2]; block[2] = block[10]; block[10] = t;
        t = block[6]; block[6] = block[14]; block[14] = t;
        t = block[3]; block[3] = block[7]; block[7] = block[11]; block[11] = block[15]; block[15] = t;

        // InvSubBytes
        for (int i = 0; i < 16; i++) {
            block[i] = inv_sbox[block[i]];
        }

        // AddRoundKey
        for (int i = 0; i < 16; i += 4) {
            uint32 k = roundKeys_[round * 4 + i/4];
            block[i] ^= (k >> 24) & 0xff;
            block[i+1] ^= (k >> 16) & 0xff;
            block[i+2] ^= (k >> 8) & 0xff;
            block[i+3] ^= k & 0xff;
        }

        // InvMixColumns (simplified)
        for (int i = 0; i < 16; i += 4) {
            uint8 a[4] = {block[i], block[i+1], block[i+2], block[i+3]};
            block[i] = a[0] ^ a[1] ^ a[2] ^ a[3];
            block[i+1] = a[0] ^ a[1] ^ a[2] ^ a[3];
            block[i+2] = a[0] ^ a[1] ^ a[2] ^ a[3];
            block[i+3] = a[0] ^ a[1] ^ a[2] ^ a[3];
        }
    }

    // Final operations
    uint8 t = block[13]; block[13] = block[9]; block[9] = block[5]; block[5] = block[1]; block[1] = t;
    t = block[2]; block[2] = block[10]; block[10] = t;
    t = block[6]; block[6] = block[14]; block[14] = t;
    t = block[3]; block[3] = block[7]; block[7] = block[11]; block[11] = block[15]; block[15] = t;

    for (int i = 0; i < 16; i++) {
        block[i] = inv_sbox[block[i]];
    }

    for (int i = 0; i < 16; i += 4) {
        uint32 k = roundKeys_[i/4];
        block[i] ^= (k >> 24) & 0xff;
        block[i+1] ^= (k >> 16) & 0xff;
        block[i+2] ^= (k >> 8) & 0xff;
        block[i+3] ^= k & 0xff;
    }
}

String AES::generateKey() {
    return ThreadSafeRandom::nextBytes(kKeySize);
}

String AES::generateIv() {
    return ThreadSafeRandom::nextBytes(kIvSize);
}

namespace aes_util {

String pkcs7Pad(const String& data, size_t blockSize) {
    size_t padLen = blockSize - (data.size() % blockSize);
    String result = data;
    result.append(padLen, static_cast<char>(padLen));
    return result;
}

String pkcs7Unpad(const String& data) {
    if (data.empty()) return "";
    size_t padLen = static_cast<uint8>(data.back());
    if (padLen > data.size() || padLen > 16) return data;
    return data.substr(0, data.size() - padLen);
}

void xorBlock(uint8* dst, const uint8* a, const uint8* b, size_t len) {
    for (size_t i = 0; i < len; i++) {
        dst[i] = a[i] ^ b[i];
    }
}

} // namespace aes_util

} // namespace legend