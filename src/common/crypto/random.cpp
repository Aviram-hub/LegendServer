/**
 * @file random.cpp
 * @brief 随机数生成器实现
 */

#include "random.h"
#include <random>
#include <sstream>
#include <iomanip>

namespace legend {

Random::Random()
    : engine_(std::random_device{}())
    , intDist_(std::numeric_limits<int32>::min(), std::numeric_limits<int32>::max())
    , doubleDist_(0.0, 1.0) {
}

Random::Random(uint32 seed)
    : engine_(seed)
    , intDist_(std::numeric_limits<int32>::min(), std::numeric_limits<int32>::max())
    , doubleDist_(0.0, 1.0) {
}

int32 Random::nextInt() {
    return intDist_(engine_);
}

int32 Random::nextInt(int32 max) {
    std::uniform_int_distribution<int32> dist(0, max - 1);
    return dist(engine_);
}

int32 Random::nextInt(int32 min, int32 max) {
    std::uniform_int_distribution<int32> dist(min, max - 1);
    return dist(engine_);
}

double Random::nextDouble() {
    return doubleDist_(engine_);
}

double Random::nextDouble(double max) {
    std::uniform_real_distribution<double> dist(0.0, max);
    return dist(engine_);
}

double Random::nextDouble(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(engine_);
}

String Random::nextString(size_t len) {
    static const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static const size_t charsLen = sizeof(chars) - 1;

    std::uniform_int_distribution<size_t> dist(0, charsLen - 1);

    String result;
    result.reserve(len);
    for (size_t i = 0; i < len; ++i) {
        result.push_back(chars[dist(engine_)]);
    }
    return result;
}

String Random::nextHexString(size_t len) {
    static const char hex[] = "0123456789abcdef";
    String result;
    result.reserve(len);
    for (size_t i = 0; i < len; ++i) {
        result.push_back(hex[engine_() % 16]);
    }
    return result;
}

void Random::nextBytes(void* buffer, size_t len) {
    std::uniform_int_distribution<int> dist(0, 255);
    uint8* p = static_cast<uint8*>(buffer);
    for (size_t i = 0; i < len; ++i) {
        p[i] = static_cast<uint8>(dist(engine_));
    }
}

String Random::nextBytes(size_t len) {
    String result;
    result.resize(len);
    nextBytes(&result[0], len);
    return result;
}

String Random::uuid() {
    std::uniform_int_distribution<uint32> dist;

    uint32 data[4];
    for (int i = 0; i < 4; ++i) {
        data[i] = dist(engine_);
    }

    // Set version (4) and variant
    data[1] = (data[1] & 0x0fffffff) | 0x40000000;
    data[2] = (data[2] & 0x3fffffff) | 0x80000000;

    std::ostringstream oss;
    oss << std::hex << std::setfill('0');

    oss << std::setw(8) << data[0] << '-';
    oss << std::setw(4) << (data[1] >> 16) << '-';
    oss << std::setw(4) << (data[1] & 0xffff) << '-';
    oss << std::setw(4) << (data[2] >> 16) << '-';
    oss << std::setw(4) << (data[2] & 0xffff);
    oss << std::setw(8) << data[3];

    return oss.str();
}

void Random::setSeed(uint32 seed) {
    engine_.seed(seed);
}

Random& ThreadSafeRandom::instance() {
    static thread_local Random random;
    return random;
}

} // namespace legend