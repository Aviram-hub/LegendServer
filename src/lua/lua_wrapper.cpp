/**
 * @file lua_wrapper.cpp
 * @brief Lua C++封装类实现
 */

#include "lua_wrapper.h"

namespace legend {

LuaStackGuard::LuaStackGuard(lua_State* L)
    : L_(L)
    , top_(lua_gettop(L_)) {
}

LuaStackGuard::~LuaStackGuard() {
    lua_settop(L_, top_);
}

LuaTable::LuaTable(lua_State* L, int index)
    : L_(L)
    , index_(index) {
}

int LuaTable::getInt(const String& key) {
    lua_pushstring(L_, key.c_str());
    lua_gettable(L_, index_ - 1);
    int value = static_cast<int>(lua_tointeger(L_, -1));
    lua_pop(L_, 1);
    return value;
}

double LuaTable::getDouble(const String& key) {
    lua_pushstring(L_, key.c_str());
    lua_gettable(L_, index_ - 1);
    double value = lua_tonumber(L_, -1);
    lua_pop(L_, 1);
    return value;
}

String LuaTable::getString(const String& key) {
    lua_pushstring(L_, key.c_str());
    lua_gettable(L_, index_ - 1);
    const char* str = lua_tostring(L_, -1);
    String value = str ? str : "";
    lua_pop(L_, 1);
    return value;
}

bool LuaTable::getBoolean(const String& key) {
    lua_pushstring(L_, key.c_str());
    lua_gettable(L_, index_ - 1);
    bool value = lua_toboolean(L_, -1) != 0;
    lua_pop(L_, 1);
    return value;
}

void LuaTable::set(const String& key, int value) {
    lua_pushstring(L_, key.c_str());
    lua_pushinteger(L_, value);
    lua_settable(L_, index_ - 2);
}

void LuaTable::set(const String& key, double value) {
    lua_pushstring(L_, key.c_str());
    lua_pushnumber(L_, value);
    lua_settable(L_, index_ - 2);
}

void LuaTable::set(const String& key, const String& value) {
    lua_pushstring(L_, key.c_str());
    lua_pushstring(L_, value.c_str());
    lua_settable(L_, index_ - 2);
}

void LuaTable::set(const String& key, bool value) {
    lua_pushstring(L_, key.c_str());
    lua_pushboolean(L_, value);
    lua_settable(L_, index_ - 2);
}

int LuaTable::getInt(int index) {
    lua_rawgeti(L_, index_, index);
    int value = static_cast<int>(lua_tointeger(L_, -1));
    lua_pop(L_, 1);
    return value;
}

String LuaTable::getString(int index) {
    lua_rawgeti(L_, index_, index);
    const char* str = lua_tostring(L_, -1);
    String value = str ? str : "";
    lua_pop(L_, 1);
    return value;
}

int LuaTable::size() {
    return static_cast<int>(lua_rawlen(L_, index_));
}

template<typename Func>
void LuaTable::foreach(Func func) {
    lua_pushnil(L_);
    while (lua_next(L_, index_ - 1) != 0) {
        String key;
        if (lua_isstring(L_, -2)) {
            key = lua_tostring(L_, -2);
        } else if (lua_isinteger(L_, -2)) {
            key = std::to_string(lua_tointeger(L_, -2));
        }

        LuaValue value(L_, -1);
        func(key, value);
        lua_pop(L_, 1);
    }
}

LuaClassRegistrar::LuaClassRegistrar(lua_State* L, const String& className)
    : L_(L)
    , className_(className) {
    // 创建元表
    luaL_newmetatable(L, className.c_str());
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
}

LuaClassRegistrar::~LuaClassRegistrar() {
    lua_pop(L_, 1);
}

} // namespace legend