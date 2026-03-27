# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Build the project
mkdir build && cd build
cmake .. && make -j$(nproc)

# Run the server
./legend_server -c ../config/server.yaml

# Run tests
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

- **Message Handling**: Register handlers via `MessageDispatcher::registerHandler(msgId, handler)`
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

Server config loaded from YAML at `config/server.yaml`. Database connections, thread counts, and game parameters are configurable.

## Dependencies

- Lua 5.4 (third_party/lua)
- spdlog (third_party/spdlog)
- yaml-cpp (third_party/yaml-cpp)
- Protobuf, MySQL client, hiredis (system packages)