/**
 * @file lua_engine.cpp
 * @brief Lua脚本引擎实现
 */

#include "lua_engine.h"
#include <fstream>
#include <sstream>

namespace legend {

LuaEngine::LuaEngine()
    : L_(luaL_newstate()) {
    luaL_openlibs(L_);
}

LuaEngine::~LuaEngine() {
    if (L_) {
        lua_close(L_);
    }
}

bool LuaEngine::loadScript(const String& filename) {
    if (luaL_loadfile(L_, filename.c_str()) != 0) {
        lastError_ = lua_tostring(L_, -1);
        lua_pop(L_, 1);
        return false;
    }

    if (lua_pcall(L_, 0, 0, 0) != 0) {
        lastError_ = lua_tostring(L_, -1);
        lua_pop(L_, 1);
        return false;
    }

    return true;
}

bool LuaEngine::loadString(const String& code) {
    if (luaL_loadstring(L_, code.c_str()) != 0) {
        lastError_ = lua_tostring(L_, -1);
        lua_pop(L_, 1);
        return false;
    }

    if (lua_pcall(L_, 0, 0, 0) != 0) {
        lastError_ = lua_tostring(L_, -1);
        lua_pop(L_, 1);
        return false;
    }

    return true;
}

bool LuaEngine::reloadScript(const String& filename) {
    // 清除已加载的模块
    lua_getglobal(L_, "package");
    lua_getfield(L_, -1, "loaded");
    lua_pushnil(L_);
    lua_setfield(L_, -2, filename.c_str());
    lua_pop(L_, 2);

    return loadScript(filename);
}

bool LuaEngine::doFile(const String& filename) {
    if (luaL_dofile(L_, filename.c_str()) != 0) {
        lastError_ = lua_tostring(L_, -1);
        lua_pop(L_, 1);
        return false;
    }
    return true;
}

bool LuaEngine::doString(const String& code) {
    if (luaL_dostring(L_, code.c_str()) != 0) {
        lastError_ = lua_tostring(L_, -1);
        lua_pop(L_, 1);
        return false;
    }
    return true;
}

bool LuaEngine::callFunction(const String& funcName) {
    lua_getglobal(L_, funcName.c_str());

    if (!lua_isfunction(L_, -1)) {
        lastError_ = funcName + " is not a function";
        lua_pop(L_, 1);
        return false;
    }

    if (lua_pcall(L_, 0, 0, 0) != 0) {
        lastError_ = lua_tostring(L_, -1);
        lua_pop(L_, 1);
        return false;
    }

    return true;
}

bool LuaEngine::callFunction(const String& funcName, const std::vector<LuaValue>& args) {
    lua_getglobal(L_, funcName.c_str());

    if (!lua_isfunction(L_, -1)) {
        lastError_ = funcName + " is not a function";
        lua_pop(L_, 1);
        return false;
    }

    for (const auto& arg : args) {
        switch (arg.type()) {
            case LuaType::NUMBER:
                lua_pushnumber(L_, arg.asNumber());
                break;
            case LuaType::STRING:
                lua_pushstring(L_, arg.asString().c_str());
                break;
            case LuaType::BOOLEAN:
                lua_pushboolean(L_, arg.asBoolean());
                break;
            default:
                lua_pushnil(L_);
                break;
        }
    }

    if (lua_pcall(L_, args.size(), 0, 0) != 0) {
        lastError_ = lua_tostring(L_, -1);
        lua_pop(L_, 1);
        return false;
    }

    return true;
}

LuaValue LuaEngine::callFunction(const String& funcName, const std::vector<LuaValue>& args, bool& success) {
    lua_getglobal(L_, funcName.c_str());

    if (!lua_isfunction(L_, -1)) {
        lastError_ = funcName + " is not a function";
        lua_pop(L_, 1);
        success = false;
        return LuaValue();
    }

    for (const auto& arg : args) {
        switch (arg.type()) {
            case LuaType::NUMBER:
                lua_pushnumber(L_, arg.asNumber());
                break;
            case LuaType::STRING:
                lua_pushstring(L_, arg.asString().c_str());
                break;
            case LuaType::BOOLEAN:
                lua_pushboolean(L_, arg.asBoolean());
                break;
            default:
                lua_pushnil(L_);
                break;
        }
    }

    if (lua_pcall(L_, args.size(), 1, 0) != 0) {
        lastError_ = lua_tostring(L_, -1);
        lua_pop(L_, 1);
        success = false;
        return LuaValue();
    }

    success = true;
    return pop();
}

void LuaEngine::registerFunction(const String& name, lua_CFunction func) {
    lua_pushcfunction(L_, func);
    lua_setglobal(L_, name.c_str());
}

void LuaEngine::setGlobal(const String& name, int value) {
    lua_pushinteger(L_, value);
    lua_setglobal(L_, name.c_str());
}

void LuaEngine::setGlobal(const String& name, double value) {
    lua_pushnumber(L_, value);
    lua_setglobal(L_, name.c_str());
}

void LuaEngine::setGlobal(const String& name, const String& value) {
    lua_pushstring(L_, value.c_str());
    lua_setglobal(L_, name.c_str());
}

void LuaEngine::setGlobal(const String& name, bool value) {
    lua_pushboolean(L_, value);
    lua_setglobal(L_, name.c_str());
}

int LuaEngine::getGlobalInt(const String& name) {
    lua_getglobal(L_, name.c_str());
    int value = static_cast<int>(lua_tointeger(L_, -1));
    lua_pop(L_, 1);
    return value;
}

double LuaEngine::getGlobalDouble(const String& name) {
    lua_getglobal(L_, name.c_str());
    double value = lua_tonumber(L_, -1);
    lua_pop(L_, 1);
    return value;
}

String LuaEngine::getGlobalString(const String& name) {
    lua_getglobal(L_, name.c_str());
    const char* str = lua_tostring(L_, -1);
    String value = str ? str : "";
    lua_pop(L_, 1);
    return value;
}

void LuaEngine::push(int value) {
    lua_pushinteger(L_, value);
}

void LuaEngine::push(double value) {
    lua_pushnumber(L_, value);
}

void LuaEngine::push(const String& value) {
    lua_pushstring(L_, value.c_str());
}

void LuaEngine::push(bool value) {
    lua_pushboolean(L_, value);
}

void LuaEngine::pushNil() {
    lua_pushnil(L_);
}

LuaValue LuaEngine::pop() {
    LuaValue value(L_, -1);
    lua_pop(L_, 1);
    return value;
}

void LuaEngine::newTable() {
    lua_newtable(L_);
}

void LuaEngine::setTableField(const String& key, int value) {
    lua_pushstring(L_, key.c_str());
    lua_pushinteger(L_, value);
    lua_settable(L_, -3);
}

void LuaEngine::setTableField(const String& key, double value) {
    lua_pushstring(L_, key.c_str());
    lua_pushnumber(L_, value);
    lua_settable(L_, -3);
}

void LuaEngine::setTableField(const String& key, const String& value) {
    lua_pushstring(L_, key.c_str());
    lua_pushstring(L_, value.c_str());
    lua_settable(L_, -3);
}

void LuaEngine::setTableField(const String& key, bool value) {
    lua_pushstring(L_, key.c_str());
    lua_pushboolean(L_, value);
    lua_settable(L_, -3);
}

int LuaEngine::getTableFieldInt(const String& key) {
    lua_pushstring(L_, key.c_str());
    lua_gettable(L_, -2);
    int value = static_cast<int>(lua_tointeger(L_, -1));
    lua_pop(L_, 1);
    return value;
}

double LuaEngine::getTableFieldDouble(const String& key) {
    lua_pushstring(L_, key.c_str());
    lua_gettable(L_, -2);
    double value = lua_tonumber(L_, -1);
    lua_pop(L_, 1);
    return value;
}

String LuaEngine::getTableFieldString(const String& key) {
    lua_pushstring(L_, key.c_str());
    lua_gettable(L_, -2);
    const char* str = lua_tostring(L_, -1);
    String value = str ? str : "";
    lua_pop(L_, 1);
    return value;
}

// LuaValue 实现
LuaValue::LuaValue()
    : type_(LuaType::NIL)
    , boolValue_(false)
    , numberValue_(0.0) {
}

LuaValue::LuaValue(lua_State* L, int index)
    : boolValue_(false)
    , numberValue_(0.0) {
    int type = lua_type(L, index);

    switch (type) {
        case LUA_TNIL:
            type_ = LuaType::NIL;
            break;
        case LUA_TBOOLEAN:
            type_ = LuaType::BOOLEAN;
            boolValue_ = lua_toboolean(L, index) != 0;
            break;
        case LUA_TNUMBER:
            type_ = LuaType::NUMBER;
            numberValue_ = lua_tonumber(L, index);
            break;
        case LUA_TSTRING:
            type_ = LuaType::STRING;
            stringValue_ = lua_tostring(L, index);
            break;
        case LUA_TTABLE:
            type_ = LuaType::TABLE;
            break;
        case LUA_TFUNCTION:
            type_ = LuaType::FUNCTION;
            break;
        case LUA_TUSERDATA:
            type_ = LuaType::USERDATA;
            break;
        case LUA_TTHREAD:
            type_ = LuaType::THREAD;
            break;
        default:
            type_ = LuaType::NIL;
            break;
    }
}

} // namespace legend