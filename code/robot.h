#pragma once

#include <cstdio>
#include <cstring>
#include <vector>

#define ROBOT_MOVE_RIGHT  0
#define ROBOT_MOVE_LEFT   1
#define ROBOT_MOVE_UP     2
#define ROBOT_MOVE_DOWN   3
#define robot_num 10

/* 
    @brief: 机器人
    @params: x,y: 机器人坐标; goods: 是否携带物品;
             status: 状态（0 表示恢复状态，1 表示正常运行状态）
    @ret: none
*/
struct Robot
{
    int x, y, goods;
    int status;
    int mbx, mby;
    Robot() {}
    Robot(int startX, int startY) {
        x = startX;
        y = startY;
    }
    int nearestberth_index;
};

/* 
    @brief: 在初始化中寻找机器人的位置
    @params: str: 
    @ret: std::vector<int>
*/
std::vector<int> GetRobotPlace(char* str);