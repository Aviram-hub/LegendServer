#!/bin/bash

# LegendServer 启动脚本

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
PROJECT_DIR=$(dirname "$SCRIPT_DIR")
BIN_DIR="$PROJECT_DIR/build/bin"
CONFIG_FILE="$PROJECT_DIR/config/server.yaml"
LOG_DIR="$PROJECT_DIR/logs"
PID_FILE="$PROJECT_DIR/legend_server.pid"

# 检查二进制文件
if [ ! -f "$BIN_DIR/legend_server" ]; then
    echo "Error: legend_server not found. Please build first."
    echo "Run: mkdir build && cd build && cmake .. && make"
    exit 1
fi

# 创建日志目录
mkdir -p "$LOG_DIR"

# 检查是否已运行
if [ -f "$PID_FILE" ]; then
    PID=$(cat "$PID_FILE")
    if ps -p $PID > /dev/null 2>&1; then
        echo "Server is already running (PID: $PID)"
        exit 1
    else
        rm -f "$PID_FILE"
    fi
fi

# 启动服务器
echo "Starting LegendServer..."
cd "$PROJECT_DIR"
nohup "$BIN_DIR/legend_server" -c "$CONFIG_FILE" > "$LOG_DIR/console.log" 2>&1 &
PID=$!

# 保存PID
echo $PID > "$PID_FILE"

echo "LegendServer started (PID: $PID)"
echo "Logs: $LOG_DIR"