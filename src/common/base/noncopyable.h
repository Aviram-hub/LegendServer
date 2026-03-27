/**
 * @file noncopyable.h
 * @brief 不可拷贝基类
 */

#pragma once

namespace legend {

/**
 * @brief 不可拷贝基类
 * 继承此类后，派生类将无法进行拷贝构造和拷贝赋值
 */
class NonCopyable {
public:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};

/**
 * @brief 不可移动基类
 * 继承此类后，派生类将无法进行移动构造和移动赋值
 */
class NonMovable {
public:
    NonMovable(NonMovable&&) = delete;
    NonMovable& operator=(NonMovable&&) = delete;

protected:
    NonMovable() = default;
    ~NonMovable() = default;
};

} // namespace legend