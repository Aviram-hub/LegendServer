/**
 * @file aoi.cpp
 * @brief AOI九宫格实现
 */

#include "aoi.h"
#include <algorithm>

namespace legend {

AOI::AOI(int32 width, int32 height, int32 gridX, int32 gridY)
    : width_(width)
    , height_(height)
    , gridX_(gridX)
    , gridY_(gridY) {
    gridWidth_ = (width + gridX - 1) / gridX;
    gridHeight_ = (height + gridY - 1) / gridY;

    grids_.resize(gridX * gridY);
}

void AOI::enter(int64 roleId, int32 x, int32 y) {
    int32 gridIndex = positionToGrid(x, y);
    grids_[gridIndex].insert(roleId);
    roleToGrid_[roleId] = gridIndex;
    rolePositions_[roleId] = {x, y};
}

void AOI::leave(int64 roleId) {
    auto it = roleToGrid_.find(roleId);
    if (it != roleToGrid_.end()) {
        grids_[it->second].erase(roleId);
        roleToGrid_.erase(it);
        rolePositions_.erase(roleId);
    }
}

void AOI::move(int64 roleId, int32 newX, int32 newY) {
    int32 newGrid = positionToGrid(newX, newY);
    auto it = roleToGrid_.find(roleId);

    if (it != roleToGrid_.end()) {
        if (it->second != newGrid) {
            grids_[it->second].erase(roleId);
            grids_[newGrid].insert(roleId);
            it->second = newGrid;
        }
        rolePositions_[roleId] = {newX, newY};
    }
}

Vector<int64> AOI::getAroundRoles(int64 roleId) {
    Vector<int64> result;

    auto it = roleToGrid_.find(roleId);
    if (it == roleToGrid_.end()) {
        return result;
    }

    auto aroundGrids = getAroundGrids(it->second);
    for (int32 gridIndex : aroundGrids) {
        for (int64 id : grids_[gridIndex]) {
            if (id != roleId) {
                result.push_back(id);
            }
        }
    }

    return result;
}

Vector<int64> AOI::getAroundRoles(int32 x, int32 y) {
    Vector<int64> result;

    int32 gridIndex = positionToGrid(x, y);
    auto aroundGrids = getAroundGrids(gridIndex);

    for (int32 grid : aroundGrids) {
        for (int64 id : grids_[grid]) {
            result.push_back(id);
        }
    }

    return result;
}

const HashSet<int64>& AOI::getRolesInGrid(int32 gridIndex) const {
    static const HashSet<int64> emptySet;

    if (gridIndex >= 0 && gridIndex < static_cast<int32>(grids_.size())) {
        return grids_[gridIndex];
    }
    return emptySet;
}

int32 AOI::getRoleGrid(int64 roleId) const {
    auto it = roleToGrid_.find(roleId);
    return it != roleToGrid_.end() ? it->second : -1;
}

int32 AOI::positionToGrid(int32 x, int32 y) const {
    int32 gridX = std::min(x / gridWidth_, gridX_ - 1);
    int32 gridY = std::min(y / gridHeight_, gridY_ - 1);
    return gridY * gridX_ + gridX;
}

Vector<int32> AOI::getAroundGrids(int32 gridIndex) const {
    Vector<int32> result;

    int32 gridY = gridIndex / gridX_;
    int32 gridX = gridIndex % gridX_;

    for (int32 dy = -1; dy <= 1; ++dy) {
        for (int32 dx = -1; dx <= 1; ++dx) {
            int32 ny = gridY + dy;
            int32 nx = gridX + dx;

            if (ny >= 0 && ny < gridY_ && nx >= 0 && nx < gridX_) {
                result.push_back(ny * gridX_ + nx);
            }
        }
    }

    return result;
}

} // namespace legend