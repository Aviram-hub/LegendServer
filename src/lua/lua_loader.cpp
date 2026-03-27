/**
 * @file lua_loader.cpp
 * @brief Lua脚本加载器实现
 */

#include "lua_loader.h"
#include <filesystem>
#include <fstream>

namespace legend {

LuaLoader::LuaLoader(LuaEngine& engine)
    : engine_(engine) {
}

void LuaLoader::addSearchPath(const String& path) {
    searchPaths_.push_back(path);

    // 更新package.path
    String packagePath = engine_.getGlobalString("package.path");
    packagePath += ";" + path + "/?.lua";
    engine_.setGlobal("package.path", packagePath);
}

bool LuaLoader::loadModule(const String& moduleName) {
    String filename = findModuleFile(moduleName);
    if (filename.empty()) {
        return false;
    }

    if (!engine_.loadScript(filename)) {
        return false;
    }

    // 记录修改时间
    auto ftime = std::filesystem::last_write_time(filename);
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
    moduleModifiedTimes_[moduleName] =
        std::chrono::duration_cast<std::chrono::milliseconds>(sctp.time_since_epoch()).count();

    return true;
}

bool LuaLoader::reloadModule(const String& moduleName) {
    // 清除已加载的模块
    engine_.doString("package.loaded['" + moduleName + "'] = nil");
    return loadModule(moduleName);
}

bool LuaLoader::loadAllModules(const String& directory) {
    namespace fs = std::filesystem;

    try {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".lua") {
                String filename = entry.path().string();
                if (!engine_.loadScript(filename)) {
                    return false;
                }
            }
        }
    } catch (...) {
        return false;
    }

    return true;
}

void LuaLoader::reloadModifiedModules() {
    for (auto& pair : moduleModifiedTimes_) {
        String filename = findModuleFile(pair.first);
        if (!filename.empty()) {
            auto ftime = std::filesystem::last_write_time(filename);
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
            int64 modifiedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                sctp.time_since_epoch()).count();

            if (modifiedTime > pair.second) {
                reloadModule(pair.first);
                pair.second = modifiedTime;
            }
        }
    }
}

String LuaLoader::findModuleFile(const String& moduleName) {
    String relativePath = moduleName;
    for (char& c : relativePath) {
        if (c == '.') c = '/';
    }
    relativePath += ".lua";

    for (const auto& path : searchPaths_) {
        String filename = path + "/" + relativePath;
        std::ifstream f(filename);
        if (f.good()) {
            return filename;
        }
    }

    return "";
}

} // namespace legend