/*
    @author: Kevin
    @date: 2024/3/11
    @brief: 利用读入的地图实现 A* 寻路算法
*/
#pragma once
#include "map.h"

#ifndef ASTAR_H
#define ASTAR_H

// 存储节点的位置、父节点和代价
struct Node 
{
    int parentX, parentY;
    int x, y;
    float f, g, h;
};

// 优先队列的比较函数
struct comp
{
    bool operator()(const Node& lhs, const Node& rhs) const
    {
        return lhs.f > rhs.f;
    }
};

/*
    @brief: A* 算法
    @param: srcX, srcY: 起始点坐标; destX, destY: 终点坐标
    @ret: none
*/
void aStarSearch(char grid[n][n], int srcX, int srcY, int destX, int destY);

#endif