#include "astar.h"
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>

using namespace std;

/*
    @brief: 判断给定节点是否为目标节点
    @param: x, y: 节点位置; destX, destY: 目标节点坐标
    @ret: true 代表为目标节点
*/
bool isDestination(int x, int y, int destX, int destY)
{
    return (x == destX && y == destY);
}

/*
    @brief: 计算曼哈顿距离
    @param: x, y: 节点位置; destX, destY: 目标节点位置
    @ret: 曼哈顿距离
*/
float calculateH(int x, int y, int destX, int destY)
{
    return static_cast<float>(abs(x - destX) + abs(y - destY));
}

/*
    @brief: 判断节点是否可达
    @param: x, y: 节点位置
    @ret: true 为可走
*/
bool isUnBlocked(char grid[N][N], int x, int y)
{
    if(x >= 0 && x < n && y >= 0 && y < n && (grid[x][y] == '*' || grid[x][y] == 'A'))
    {
        return true;
    }
    return false;
}

/*
    @brief: A* 算法
    @param: srcX, srcY: 起始点坐标; destX, destY: 终点坐标
    @ret: none
*/
void aStarSearch(char grid[N][N], int srcX, int srcY, int destX, int destY)
{
    // 若起点或终点不可达
    if(!isUnBlocked(grid, srcX, srcY) || !isUnBlocked(grid, destX, destY))
    {
        return;
    }

    bool closedList[n][n];

    memset(closedList, false, sizeof(closedList));
    Node nodes[n][n];

    int dx[] = {-1,  1,  0, 0};
    int dy[] = { 0,  0, -1, 1}; // 上下左右

    // Initialize nodes.
    for(int x = 0; x < n; ++x)
    {
        for(int y = 0; y < n; ++y)
        {
            nodes[x][y].f = FLT_MAX;
            nodes[x][y].g = FLT_MAX;
            nodes[x][y].h = FLT_MAX;
            nodes[x][y].x = x;
            nodes[x][y].y = y;
            nodes[x][y].parentX = -1;
            nodes[x][y].parentY = -1;
        }
    }

    // Initialize source position.
    int i = srcX, j = srcY;
    nodes[i][j].f = 0.0;
    nodes[i][j].g = 0.0;
    nodes[i][j].h = 0.0;
    nodes[i][j].parentX = i;
    nodes[i][j].parentY = j;

    priority_queue<Node, vector<Node>, comp> openList; // 按 f 值排序
    openList.push(nodes[i][j]);

    while(!openList.empty())
    {
        const Node node = openList.top();
        openList.pop();
        i = node.x;
        j = node.y;
        closedList[i][j] = true;

        for(int k = 0; k < 4; ++k)
        {
            int newX = i + dx[k];
            int newY = j + dy[k];

            if(isDestination(newX, newY, destX, destY))
            {
                return;
            }
            
            // Check if the node can be calculated.
            if(!isUnBlocked(grid, newX, newY) && !closedList[newX][newY])
            {
                float gNew = nodes[i][i].g + 1.0;
                float hNew = calculateH(newX, newY, destX, destY);
                float fNew = gNew + hNew;

                if(nodes[newX][newY].f > fNew)
                {
                    if(nodes[newX][newY].f == FLT_MAX)
                    {
                        openList.push(nodes[newX][newY]);
                    }
                    nodes[newX][newY].f = fNew;
                    nodes[newX][newY].g = gNew;
                    nodes[newX][newY].h = hNew;
                    nodes[newX][newY].parentX = i;
                    nodes[newX][newY].parentY = j;
                }
            }
        }
    }

}