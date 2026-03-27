cmake_minimum_required(VERSION 3.16)

# 测试源文件
set(TEST_SOURCES
    test_thread_pool.cpp
    test_mysql_pool.cpp
)

# 通用测试链接库
set(TEST_LIBS
    ${CMAKE_THREAD_LIBS_INIT}
    ${LUA_LIBRARY}
    ${YAML_CPP_LIBRARY}
    ${Protobuf_LIBRARIES}
    mysqlclient
    hiredis
    dl
    rt
)

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${LUA_INCLUDE_DIR}
    ${SPDLOG_INCLUDE_DIR}
    ${YAML_CPP_INCLUDE_DIR}
)

# 创建测试可执行文件
foreach(TEST_SOURCE ${TEST_SOURCES})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_SOURCE})
        target_link_libraries(${TEST_NAME} ${TEST_LIBS})
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endif()
endforeach()