cmake_minimum_required(VERSION 3.16)
project(LegendServer VERSION 1.0.0 LANGUAGES CXX)

# C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# 查找依赖库
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)

# Lua配置
set(LUA_DIR ${CMAKE_SOURCE_DIR}/third_party/lua)
set(LUA_INCLUDE_DIR ${LUA_DIR}/src)
set(LUA_LIBRARY ${LUA_DIR}/liblua.a)

# spdlog配置
set(SPDLOG_DIR ${CMAKE_SOURCE_DIR}/third_party/spdlog)
set(SPDLOG_INCLUDE_DIR ${SPDLOG_DIR}/include)

# yaml-cpp配置
set(YAML_CPP_DIR ${CMAKE_SOURCE_DIR}/third_party/yaml-cpp)
set(YAML_CPP_INCLUDE_DIR ${YAML_CPP_DIR}/include)
set(YAML_CPP_LIBRARY ${YAML_CPP_DIR}/lib/libyaml-cpp.a)

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${LUA_INCLUDE_DIR}
    ${SPDLOG_INCLUDE_DIR}
    ${YAML_CPP_INCLUDE_DIR}
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
    protobuf::protobuf
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