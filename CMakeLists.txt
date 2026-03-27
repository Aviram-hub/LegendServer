cmake_minimum_required(VERSION 3.16)
project(LegendServer VERSION 1.0.0 LANGUAGES CXX)

# C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# 查找依赖库
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)

# Lua - 使用系统库
find_package(Lua QUIET)
if(NOT LUA_FOUND)
    find_path(LUA_INCLUDE_DIR lua.h PATHS /usr/include/lua5.4 /usr/include/lua5.3 /usr/include)
    find_library(LUA_LIBRARY NAMES lua5.4 lua5.3 lua PATHS /usr/lib)
endif()

# spdlog - 使用系统库
find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
    find_path(SPDLOG_INCLUDE_DIR spdlog/spdlog.h PATHS /usr/include)
endif()

# yaml-cpp - 使用系统库
find_package(yaml-cpp QUIET)
if(yaml-cpp_FOUND)
    set(YAML_CPP_LIBRARY yaml-cpp)
else()
    find_path(YAML_CPP_INCLUDE_DIR yaml-cpp/yaml.h PATHS /usr/include)
    find_library(YAML_CPP_LIBRARY NAMES yaml-cpp PATHS /usr/lib /usr/lib/x86_64-linux-gnu)
endif()

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${LUA_INCLUDE_DIR}
    ${SPDLOG_INCLUDE_DIR}
    ${YAML_CPP_INCLUDE_DIR}
    ${Protobuf_INCLUDE_DIRS}
)

# 源文件
file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

# 主可执行文件
add_executable(legend_server ${SOURCES})

# 链接库
target_link_libraries(legend_server
    ${CMAKE_THREAD_LIBS_INIT}
    ${LUA_LIBRARY}
    ${YAML_CPP_LIBRARY}
    ${Protobuf_LIBRARIES}
    mysqlclient
    hiredis
    dl
    rt
)

# 单元测试
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 安装配置
install(TARGETS legend_server DESTINATION bin)
install(DIRECTORY lua_scripts/ DESTINATION share/legend_server/lua_scripts)
install(DIRECTORY config/ DESTINATION etc/legend_server)