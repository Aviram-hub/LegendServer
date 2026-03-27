/**
 * @file singleton.h
 * @brief 单例模式模板
 */

#pragma once

#include "noncopyable.h"
#include <memory>
#include <mutex>

namespace legend {

/**
 * @brief 单例模板类 (线程安全)
 * @tparam T 实际类型
 */
template<typename T>
class Singleton : public NonCopyable {
public:
    /**
     * @brief 获取单例实例
     */
    static T& instance() {
        static std::once_flag once;
        std::call_once(once, []() {
            instance_.reset(new T());
        });
        return *instance_;
    }

protected:
    Singleton() = default;
    ~Singleton() = default;

private:
    static std::unique_ptr<T> instance_;
};

template<typename T>
std::unique_ptr<T> Singleton<T>::instance_ = nullptr;

/**
 * @brief 单例模板类 (可手动创建销毁)
 * @tparam T 实际类型
 */
template<typename T>
class ManagedSingleton : public NonCopyable {
public:
    /**
     * @brief 创建单例实例
     * @tparam Args 构造函数参数类型
     * @param args 构造函数参数
     * @return T& 实例引用
     */
    template<typename... Args>
    static T& create(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
            instance_.reset(new T(std::forward<Args>(args)...));
        }
        return *instance_;
    }

    /**
     * @brief 销毁单例实例
     */
    static void destroy() {
        std::lock_guard<std::mutex> lock(mutex_);
        instance_.reset();
    }

    /**
     * @brief 获取单例实例
     * @return T* 实例指针，可能为空
     */
    static T* get() {
        std::lock_guard<std::mutex> lock(mutex_);
        return instance_.get();
    }

    /**
     * @brief 检查单例是否存在
     */
    static bool exists() {
        std::lock_guard<std::mutex> lock(mutex_);
        return instance_ != nullptr;
    }

protected:
    ManagedSingleton() = default;
    ~ManagedSingleton() = default;

private:
    static std::unique_ptr<T> instance_;
    static std::mutex mutex_;
};

template<typename T>
std::unique_ptr<T> ManagedSingleton<T>::instance_ = nullptr;

template<typename T>
std::mutex ManagedSingleton<T>::mutex_;

} // namespace legend