/**
 * @file lua_engine.h
 * @brief Lua脚本引擎
 */

#pragma once

#include "types.h"
#include "noncopyable.h"
#include <lua.hpp>
#include <functional>

namespace legend {

/**
 * @brief Lua值类型
 */
enum class LuaType {
    NIL,
    BOOLEAN,
    NUMBER,
    STRING,
    TABLE,
    FUNCTION,
    USERDATA,
    THREAD
};

/**
 * @brief Lua值
 */
class LuaValue {
public:
    LuaValue();
    explicit LuaValue(lua_State* L, int index);

    LuaType type() const { return type_; }
    bool isNil() const { return type_ == LuaType::NIL; }
    bool isBoolean() const { return type_ == LuaType::BOOLEAN; }
    bool isNumber() const { return type_ == LuaType::NUMBER; }
    bool isString() const { return type_ == LuaType::STRING; }
    bool isTable() const { return type_ == LuaType::TABLE; }
    bool isFunction() const { return type_ == LuaType::FUNCTION; }

    bool asBoolean() const { return boolValue_; }
    double asNumber() const { return numberValue_; }
    String asString() const { return stringValue_; }

private:
    LuaType type_;
    bool boolValue_;
    double numberValue_;
    String stringValue_;
};

/**
 * @brief Lua脚本引擎
 */
class LuaEngine : public NonCopyable {
public:
    LuaEngine();
    ~LuaEngine();

    // 获取Lua状态
    lua_State* state() { return L_; }

    // 加载脚本
    bool loadScript(const String& filename);
    bool loadString(const String& code);

    // 重新加载脚本
    bool reloadScript(const String& filename);

    // 执行脚本
    bool doFile(const String& filename);
    bool doString(const String& code);

    // 调用函数
    bool callFunction(const String& funcName);
    bool callFunction(const String& funcName, const std::vector<LuaValue>& args);
    LuaValue callFunction(const String& funcName, const std::vector<LuaValue>& args, bool& success);

    // 注册C++函数
    void registerFunction(const String& name, lua_CFunction func);

    // 注册全局变量
    void setGlobal(const String& name, int value);
    void setGlobal(const String& name, double value);
    void setGlobal(const String& name, const String& value);
    void setGlobal(const String& name, bool value);

    // 获取全局变量
    int getGlobalInt(const String& name);
    double getGlobalDouble(const String& name);
    String getGlobalString(const String& name);

    // 错误处理
    String getLastError() const { return lastError_; }

    // 栈操作
    void push(int value);
    void push(double value);
    void push(const String& value);
    void push(bool value);
    void pushNil();

    LuaValue pop();

    // 表操作
    void newTable();
    void setTableField(const String& key, int value);
    void setTableField(const String& key, double value);
    void setTableField(const String& key, const String& value);
    void setTableField(const String& key, bool value);

    int getTableFieldInt(const String& key);
    double getTableFieldDouble(const String& key);
    String getTableFieldString(const String& key);

private:
    lua_State* L_;
    String lastError_;
    HashMap<String, int64> fileModifiedTimes_;
};

} // namespace legend