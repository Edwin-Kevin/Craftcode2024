/*
    @author: Kevin
    @date: 2024/3/11
    @brief: 利用读入的地图实现 A* 寻路算法
*/
#pragma once
#include "map.h"

#ifndef ASTAR_H
#define ASTAR_H

/*
    @brief: A* 算法
    @param: srcX, srcY: 起始点坐标; destX, destY: 终点坐标
    @ret: none
*/
std::vector<std::pair<int, int>> aStarSearch(char grid[n][n], int srcX, int srcY, int destX, int destY);

#endif