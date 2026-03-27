cmake_minimum_required(VERSION 3.16)

# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# 公共模块源文件
set(COMMON_SOURCES
    common/base/buffer.cpp
    common/thread/thread.cpp
    common/thread/mutex.cpp
    common/thread/condition.cpp
    common/thread/thread_pool.cpp
    common/thread/task_queue.cpp
    common/network/socket.cpp
    common/network/address.cpp
    common/network/connection.cpp
    common/network/buffer_stream.cpp
    common/timer/timer.cpp
    common/timer/timer_manager.cpp
    common/crypto/md5.cpp
    common/crypto/aes.cpp
    common/crypto/random.cpp
)

# 核心模块源文件
set(CORE_SOURCES
    core/reactor/epoll_poller.cpp
    core/reactor/event_loop.cpp
    core/reactor/channel.cpp
    core/server/server.cpp
    core/server/server_config.cpp
    core/session/session.cpp
    core/session/session_manager.cpp
    core/message/message.cpp
    core/message/message_queue.cpp
    core/message/codec.cpp
    core/message/dispatcher.cpp
)

# Lua引擎源文件
set(LUA_SOURCES
    lua/lua_engine.cpp
    lua/lua_wrapper.cpp
    lua/lua_table.cpp
    lua/lua_loader.cpp
)

# 数据库模块源文件
set(DATABASE_SOURCES
    database/mysql/mysql_connection.cpp
    database/mysql/mysql_connection_pool.cpp
    database/mysql/mysql_result.cpp
    database/mysql/mysql_statement.cpp
    database/redis/redis_client.cpp
    database/redis/redis_pool.cpp
    database/orm/entity.cpp
    database/orm/entity_manager.cpp
    database/orm/query_builder.cpp
)

# 网关模块源文件
set(GATEWAY_SOURCES
    gateway/gateway_server.cpp
    gateway/gateway_config.cpp
    gateway/client_connection.cpp
    gateway/message_router.cpp
)

# 游戏模块源文件
set(GAME_SOURCES
    game/game_server.cpp
    game/game_config.cpp
    game/modules/login/login_module.cpp
    game/modules/login/login_handler.cpp
    game/modules/role/role_module.cpp
    game/modules/role/role_manager.cpp
    game/modules/role/role_data.cpp
    game/modules/battle/battle_module.cpp
    game/modules/battle/battle_manager.cpp
    game/modules/battle/battle_scene.cpp
    game/modules/battle/skill_system.cpp
    game/modules/chat/chat_module.cpp
    game/modules/chat/chat_channel.cpp
    game/modules/friend/friend_module.cpp
    game/modules/friend/friend_manager.cpp
    game/modules/team/team_module.cpp
    game/modules/team/team_manager.cpp
    game/scene/scene.cpp
    game/scene/scene_manager.cpp
    game/scene/aoi.cpp
)

# 合并所有源文件
set(ALL_SOURCES
    ${COMMON_SOURCES}
    ${CORE_SOURCES}
    ${LUA_SOURCES}
    ${DATABASE_SOURCES}
    ${GATEWAY_SOURCES}
    ${GAME_SOURCES}
    main.cpp
)

# 创建库
add_library(legend_common STATIC ${COMMON_SOURCES})
add_library(legend_core STATIC ${CORE_SOURCES})
add_library(legend_lua STATIC ${LUA_SOURCES})
add_library(legend_database STATIC ${DATABASE_SOURCES})
add_library(legend_gateway STATIC ${GATEWAY_SOURCES})
add_library(legend_game STATIC ${GAME_SOURCES})

# 链接顺序很重要
target_link_libraries(legend_core legend_common)
target_link_libraries(legend_lua legend_common)
target_link_libraries(legend_database legend_common)
target_link_libraries(legend_gateway legend_core legend_common)
target_link_libraries(legend_game legend_core legend_lua legend_database legend_common)