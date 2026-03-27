/**
 * @file aoi.h
 * @brief AOI(Area of Interest)九宫格系统
 */

#pragma once

#include "common/base/types.h"
#include "common/base/noncopyable.h"
#include <vector>
#include <unordered_set>

namespace legend {

/**
 * @brief AOI九宫格系统
 * 高效计算视野内的对象
 */
class AOI : public NonCopyable {
public:
    /**
     * @brief 构造函数
     * @param width 场景宽度
     * @param height 场景高度
     * @param gridX X方向格子数
     * @param gridY Y方向格子数
     */
    AOI(int32 width, int32 height, int32 gridX, int32 gridY);
    ~AOI() = default;

    // 进入场景
    void enter(int64 roleId, int32 x, int32 y);

    // 离开场景
    void leave(int64 roleId);

    // 移动
    void move(int64 roleId, int32 newX, int32 newY);

    // 获取九宫格内的角色
    Vector<int64> getAroundRoles(int64 roleId);

    // 获取指定位置周围的角色
    Vector<int64> getAroundRoles(int32 x, int32 y);

    // 获取格子内的角色
    const HashSet<int64>& getRolesInGrid(int32 gridIndex) const;

    // 获取角色所在格子
    int32 getRoleGrid(int64 roleId) const;

private:
    // 坐标转格子索引
    int32 positionToGrid(int32 x, int32 y) const;

    // 获取九宫格格子索引
    Vector<int32> getAroundGrids(int32 gridIndex) const;

    int32 width_;
    int32 height_;
    int32 gridX_;
    int32 gridY_;
    int32 gridWidth_;
    int32 gridHeight_;

    // 格子到角色的映射
    Vector<HashSet<int64>> grids_;

    // 角色到格子的映射
    HashMap<int64, int32> roleToGrid_;

    // 角色坐标
    HashMap<int64, std::pair<int32, int32>> rolePositions_;
};

} // namespace legend