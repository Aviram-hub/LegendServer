/**
 * @file timer.cpp
 * @brief 定时器实现
 */

#include "timer.h"

namespace legend {

std::atomic<int64> Timer::s_numCreated_(0);

} // namespace legend