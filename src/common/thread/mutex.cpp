/**
 * @file mutex.cpp
 * @brief 互斥锁实现
 */

#include "mutex.h"

// 大部分实现都在头文件中，这里只保留编译单元
namespace legend {

// 错误检查属性
#ifdef DEBUG
#define CHECK_MUTEX_INIT(mutex) \
    do { \
        int ret = pthread_mutexattr_init(&attr); \
        if (ret != 0) { /* error */ } \
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK); \
        pthread_mutex_init(mutex, &attr); \
        pthread_mutexattr_destroy(&attr); \
    } while(0)
#else
#define CHECK_MUTEX_INIT(mutex) pthread_mutex_init(mutex, nullptr)
#endif

} // namespace legend