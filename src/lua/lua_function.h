/**
 * @file lua_function.h
 * @brief Lua函数绑定
 */

#pragma once

#include "common/base/types.h"
#include <lua.hpp>
#include <functional>

namespace legend {

/**
 * @brief Lua函数绑定器
 */
template<typename Func>
class LuaFunctionBinder;

// 特化：无参数函数
template<typename R>
class LuaFunctionBinder<std::function<R()>> {
public:
    static int call(lua_State* L, std::function<R()> func) {
        R result = func();
        pushValue(L, result);
        return 1;
    }
};

// 特化：一个参数函数
template<typename R, typename A1>
class LuaFunctionBinder<std::function<R(A1)>> {
public:
    static int call(lua_State* L, std::function<R(A1)> func) {
        A1 a1 = getValue<A1>(L, 1);
        R result = func(a1);
        pushValue(L, result);
        return 1;
    }
};

// 特化：两个参数函数
template<typename R, typename A1, typename A2>
class LuaFunctionBinder<std::function<R(A1, A2)>> {
public:
    static int call(lua_State* L, std::function<R(A1, A2)> func) {
        A1 a1 = getValue<A1>(L, 1);
        A2 a2 = getValue<A2>(L, 2);
        R result = func(a1, a2);
        pushValue(L, result);
        return 1;
    }
};

// 特化：三个参数函数
template<typename R, typename A1, typename A2, typename A3>
class LuaFunctionBinder<std::function<R(A1, A2, A3)>> {
public:
    static int call(lua_State* L, std::function<R(A1, A2, A3)> func) {
        A1 a1 = getValue<A1>(L, 1);
        A2 a2 = getValue<A2>(L, 2);
        A3 a3 = getValue<A3>(L, 3);
        R result = func(a1, a2, a3);
        pushValue(L, result);
        return 1;
    }
};

// 辅助函数：从栈获取值
template<typename T>
T getValue(lua_State* L, int index);

template<> inline int getValue<int>(lua_State* L, int index) {
    return static_cast<int>(lua_tointeger(L, index));
}

template<> inline double getValue<double>(lua_State* L, int index) {
    return lua_tonumber(L, index);
}

template<> inline String getValue<String>(lua_State* L, int index) {
    const char* str = lua_tostring(L, index);
    return str ? str : "";
}

template<> inline bool getValue<bool>(lua_State* L, int index) {
    return lua_toboolean(L, index) != 0;
}

// 辅助函数：压入值到栈
inline void pushValue(lua_State* L, int value) {
    lua_pushinteger(L, value);
}

inline void pushValue(lua_State* L, double value) {
    lua_pushnumber(L, value);
}

inline void pushValue(lua_State* L, const String& value) {
    lua_pushstring(L, value.c_str());
}

inline void pushValue(lua_State* L, bool value) {
    lua_pushboolean(L, value);
}

} // namespace legend