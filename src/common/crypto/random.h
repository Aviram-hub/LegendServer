/**
 * @file random.h
 * @brief 随机数生成器
 */

#pragma once

#include "common/base/types.h"
#include <random>

namespace legend {

/**
 * @brief 随机数生成器
 */
class Random {
public:
    Random();
    explicit Random(uint32 seed);

    // 生成随机整数
    int32 nextInt();
    int32 nextInt(int32 max);
    int32 nextInt(int32 min, int32 max);

    // 生成随机浮点数
    double nextDouble();
    double nextDouble(double max);
    double nextDouble(double min, double max);

    // 生成随机字符串
    String nextString(size_t len);
    String nextHexString(size_t len);

    // 生成随机字节
    void nextBytes(void* buffer, size_t len);
    String nextBytes(size_t len);

    // 生成UUID
    String uuid();

    // 重置种子
    void setSeed(uint32 seed);

private:
    std::mt19937 engine_;
    std::uniform_int_distribution<int32> intDist_;
    std::uniform_real_distribution<double> doubleDist_;
};

/**
 * @brief 线程安全的随机数生成器
 */
class ThreadSafeRandom {
public:
    static Random& instance();

    static int32 nextInt() { return instance().nextInt(); }
    static int32 nextInt(int32 max) { return instance().nextInt(max); }
    static int32 nextInt(int32 min, int32 max) { return instance().nextInt(min, max); }

    static double nextDouble() { return instance().nextDouble(); }
    static double nextDouble(double max) { return instance().nextDouble(max); }
    static double nextDouble(double min, double max) { return instance().nextDouble(min, max); }

    static String nextString(size_t len) { return instance().nextString(len); }
    static String nextHexString(size_t len) { return instance().nextHexString(len); }

    static void nextBytes(void* buffer, size_t len) { instance().nextBytes(buffer, len); }
    static String nextBytes(size_t len) { return instance().nextBytes(len); }

    static String uuid() { return instance().uuid(); }
};

} // namespace legend