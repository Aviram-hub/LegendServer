/**
 * @file types.h
 * @brief 基础类型定义
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <memory>
#include <functional>
#include <chrono>
#include <atomic>

namespace legend {

// 基础类型别名
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

// 字符串类型
using String = std::string;
using StringView = std::string_view;

// 容器类型
template<typename T>
using Vector = std::vector<T>;

template<typename K, typename V>
using Map = std::map<K, V>;

template<typename K, typename V>
using HashMap = std::unordered_map<K, V>;

template<typename T>
using Set = std::set<T>;

template<typename T>
using HashSet = std::unordered_set<T>;

// 智能指针
template<typename T>
using Ptr = std::shared_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T, typename... Args>
Ptr<T> makePtr(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

// 函数类型
template<typename T>
using Function = std::function<T>;

// 时间类型
using TimePoint = std::chrono::steady_clock::time_point;
using Duration = std::chrono::nanoseconds;

inline TimePoint now() {
    return std::chrono::steady_clock::now();
}

inline int64 nowMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

inline int64 nowUs() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

// 错误码定义
enum class ErrorCode : int32 {
    SUCCESS = 0,

    // 通用错误 1-99
    UNKNOWN_ERROR = 1,
    INVALID_PARAM = 2,
    INTERNAL_ERROR = 3,
    TIMEOUT = 4,
    NOT_FOUND = 5,

    // 登录错误 100-199
    ACCOUNT_NOT_FOUND = 100,
    PASSWORD_ERROR = 101,
    ACCOUNT_BANNED = 102,
    ALREADY_LOGIN = 103,
    TOKEN_EXPIRED = 104,
    SERVER_FULL = 105,

    // 角色错误 200-299
    ROLE_NOT_FOUND = 200,
    ROLE_NAME_EXISTS = 201,
    ROLE_LIMIT_REACHED = 202,
    ROLE_LEVEL_LIMIT = 203,

    // 战斗错误 300-399
    BATTLE_NOT_FOUND = 300,
    SKILL_NOT_LEARNED = 301,
    SKILL_COOLDOWN = 302,
    MP_NOT_ENOUGH = 303,
    TARGET_NOT_IN_RANGE = 304,

    // 聊天错误 400-499
    CHAT_COOLDOWN = 400,
    CHAT_CONTENT_TOO_LONG = 401,

    // 好友错误 500-599
    FRIEND_NOT_FOUND = 500,
    FRIEND_LIMIT_REACHED = 501,
    ALREADY_FRIEND = 502,

    // 组队错误 600-699
    TEAM_NOT_FOUND = 600,
    TEAM_FULL = 601,
    NOT_IN_TEAM = 602,
    ALREADY_IN_TEAM = 603,
};

// 消息ID定义
enum class MessageId : uint32 {
    // 登录模块 1000-1999
    MSG_LOGIN_REQUEST = 1000,
    MSG_LOGIN_RESPONSE = 1001,
    MSG_LOGOUT_REQUEST = 1002,
    MSG_HEARTBEAT = 1003,

    // 角色模块 2000-2999
    MSG_CREATE_ROLE_REQUEST = 2000,
    MSG_CREATE_ROLE_RESPONSE = 2001,
    MSG_ROLE_INFO_NOTIFY = 2002,
    MSG_ROLE_ATTRIBUTE_UPDATE = 2003,

    // 战斗模块 3000-3999
    MSG_BATTLE_START_REQUEST = 3000,
    MSG_BATTLE_START_RESPONSE = 3001,
    MSG_SKILL_CAST_REQUEST = 3002,
    MSG_SKILL_CAST_NOTIFY = 3003,
    MSG_BATTLE_END_NOTIFY = 3004,

    // 聊天模块 4000-4999
    MSG_CHAT_REQUEST = 4000,
    MSG_CHAT_NOTIFY = 4001,

    // 好友模块 5000-5999
    MSG_ADD_FRIEND_REQUEST = 5000,
    MSG_ADD_FRIEND_RESPONSE = 5001,
    MSG_FRIEND_LIST_REQUEST = 5002,
    MSG_FRIEND_LIST_RESPONSE = 5003,

    // 组队模块 6000-6999
    MSG_CREATE_TEAM_REQUEST = 6000,
    MSG_JOIN_TEAM_REQUEST = 6001,
    MSG_LEAVE_TEAM_REQUEST = 6002,
};

// 协议头魔数
constexpr uint32 MAGIC_NUMBER = 0x4C475356;  // "LGSV"
constexpr uint8 PROTOCOL_VERSION = 1;
constexpr uint32 HEADER_SIZE = 13;

} // namespace legend