/**
 * @file lua_wrapper.h
 * @brief Lua C++封装类
 */

#pragma once

#include "types.h"
#include "lua_engine.h"
#include <lua.hpp>

namespace legend {

/**
 * @brief Lua栈守卫
 */
class LuaStackGuard {
public:
    explicit LuaStackGuard(lua_State* L);
    ~LuaStackGuard();

private:
    lua_State* L_;
    int top_;
};

/**
 * @brief Lua表操作类
 */
class LuaTable {
public:
    LuaTable(lua_State* L, int index = -1);

    // 获取字段
    int getInt(const String& key);
    double getDouble(const String& key);
    String getString(const String& key);
    bool getBoolean(const String& key);

    // 设置字段
    void set(const String& key, int value);
    void set(const String& key, double value);
    void set(const String& key, const String& value);
    void set(const String& key, bool value);

    // 数组操作
    int getInt(int index);
    String getString(int index);
    int size();

    // 遍历
    template<typename Func>
    void foreach(Func func);

private:
    lua_State* L_;
    int index_;
};

/**
 * @brief Lua类注册器
 */
class LuaClassRegistrar {
public:
    LuaClassRegistrar(lua_State* L, const String& className);
    ~LuaClassRegistrar();

    template<typename T>
    LuaClassRegistrar& addConstructor();

    template<typename T, typename Ret, typename... Args>
    LuaClassRegistrar& addMethod(const String& name, Ret (T::*method)(Args...));

    template<typename T>
    LuaClassRegistrar& addProperty(const String& name, T T::*prop);

    template<typename Func>
    LuaClassRegistrar& addStaticMethod(const String& name, Func func);

private:
    lua_State* L_;
    String className_;
};

} // namespace legend