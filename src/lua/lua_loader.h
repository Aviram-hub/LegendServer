/**
 * @file lua_loader.h
 * @brief Lua脚本加载器
 */

#pragma once

#include "types.h"
#include "lua_engine.h"
#include <unordered_map>

namespace legend {

/**
 * @brief Lua脚本加载器
 */
class LuaLoader {
public:
    LuaLoader(LuaEngine& engine);
    ~LuaLoader() = default;

    // 添加搜索路径
    void addSearchPath(const String& path);

    // 加载模块
    bool loadModule(const String& moduleName);

    // 重新加载模块
    bool reloadModule(const String& moduleName);

    // 加载所有模块
    bool loadAllModules(const String& directory);

    // 重新加载修改过的模块
    void reloadModifiedModules();

    // 获取模块
    bool getModule(const String& moduleName);

private:
    String findModuleFile(const String& moduleName);

    LuaEngine& engine_;
    std::vector<String> searchPaths_;
    HashMap<String, int64> moduleModifiedTimes_;
};

} // namespace legend