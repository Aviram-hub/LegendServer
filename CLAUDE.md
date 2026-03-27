# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Build the project
mkdir build && cd build
cmake .. && make -j$(nproc)

# Run the server
./legend_server -c ../config/server.yaml

# Run tests (via CTest)
cd build
ctest --output-on-failure

# Run individual tests
./test_thread_pool
./test_mysql_pool
```

## Architecture Overview

LegendServer is a high-performance MMORPG game server using C++17/Lua hybrid architecture.

### Layer Structure

1. **Gateway Layer** (`src/gateway/`) - Connection management, message routing, protocol parsing
2. **Game Logic Layer** (`src/game/`) - Business modules (login, role, battle, chat, friend, team)
3. **Data Layer** (`src/database/`) - MySQL connection pool, Redis client, ORM abstraction

### Core Components

- **Reactor Pattern** (`src/core/reactor/`): epoll-based event loop with Channel abstraction
- **Lua Engine** (`src/lua/`): C++/Lua binding with hot-reload support via `LuaEngine` class
- **Message System** (`src/core/message/`): Protobuf serialization, codec, and dispatcher pattern
- **Session Management** (`src/core/session/`): Thread-safe player session tracking

### Key Patterns

- **Message Handling**: Use `REGISTER_HANDLER(msgId, handler)` macro to register message handlers at static initialization time
- **Database Access**: Use `MySQLConnectionGuard` for RAII-style connection management
- **Lua Integration**: Load scripts via `LuaEngine::loadScript()`, call functions via `LuaEngine::callFunction()`
- **Threading**: Use `ThreadPool::submit()` for async tasks, `EventLoop::runInLoop()` for cross-thread dispatch

### Protocol Format

13-byte header: `[magic(4B)][version(1B)][length(4B)][messageId(4B)]` + Protobuf body

Magic number: `0x4C475356` ("LGSV")

### Entity System

ORM pattern in `src/database/orm/`:
- Entities extend `Entity` base class
- Dirty field tracking for incremental updates
- Use `EntityManager` for caching and lifecycle management

### AOI (Area of Interest)

Nine-grid algorithm in `src/game/scene/aoi.h`:
- Grid-based spatial partitioning
- `AOI::getAroundRoles()` returns entities in 3x3 grid around position

## Configuration

Server config loaded from YAML at `config/server.yaml`. Key sections:
- `server.gateway`/`server.game`: Network ports and connection limits
- `server.threads`: IO/logic/DB thread counts
- `database.mysql`/`database.redis`: Connection pool settings
- `game.*`: Game-specific parameters (role limits, battle rounds, chat cooldowns)

## Dependencies

System packages (apt install):
- lua5.4, liblua5.4-dev
- libprotobuf-dev, protobuf-compiler
- libyaml-cpp-dev
- libspdlog-dev
- libmysqlclient-dev
- libhiredis-dev

## Code Conventions

- **Namespace**: All code under `legend` namespace
- **Types**: Use type aliases from `common/base/types.h` (String, Vector, HashMap, Ptr, UniquePtr, etc.)
- **Base class**: Inherit from `NonCopyable` to disable copy semantics
- **Singleton**: Use `Singleton<T>::instance()` for singleton access
- **Naming**: PascalCase for classes/types, camelCase for functions/variables, UPPER_CASE for constants

## Server Lifecycle

1. `main.cpp` creates `EventLoop` and `Server`
2. `Server::init()` initializes database pools and Lua engine
3. `Server::start()` starts gateway and game server listeners
4. Event loop runs until SIGINT/SIGTERM received
5. `EventLoop::quit()` triggers graceful shutdown