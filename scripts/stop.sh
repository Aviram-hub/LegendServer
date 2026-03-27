#!/bin/bash

# LegendServer 停止脚本

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd")
PROJECT_DIR=$(dirname "$SCRIPT_DIR")
PID_FILE="$PROJECT_DIR/legend_server.pid"

# 检查PID文件
if [ ! -f "$PID_FILE" ]; then
    echo "Server is not running (no PID file found)"
    exit 0
fi

PID=$(cat "$PID_FILE")

# 检查进程是否存在
if ! ps -p $PID > /dev/null 2>&1; then
    echo "Server is not running (process $PID not found)"
    rm -f "$PID_FILE"
    exit 0
fi

# 发送SIGTERM信号
echo "Stopping LegendServer (PID: $PID)..."
kill -TERM $PID

# 等待进程退出
for i in {1..10}; do
    if ! ps -p $PID > /dev/null 2>&1; then
        echo "Server stopped"
        rm -f "$PID_FILE"
        exit 0
    fi
    sleep 1
done

# 强制杀死
echo "Force killing server..."
kill -9 $PID
rm -f "$PID_FILE"
echo "Server killed"