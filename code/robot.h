#pragma once

#include <cstdio>
#include <cstring>
#include <vector>
#include "map.h"

#define ROBOT_MOVE_RIGHT  0
#define ROBOT_MOVE_LEFT   1
#define ROBOT_MOVE_UP     2
#define ROBOT_MOVE_DOWN   3
#define robot_num 10

/* 
    @brief: 机器人
    @params: x,y: 机器人坐标; goods: 是否携带物品(0 表示未携带，1 表示已携带);
             status: 状态(0 表示恢复状态，1 表示正常运行状态);
             mbx, mby: 下一步行动的位置;
             enable: 是否使能
    @ret: none
*/
struct Robot
{
    int x, y, goods;
    int status;
    int mbx, mby;
    bool enable = false;
    Robot() {}
    Robot(int startX, int startY) {
        x = startX;
        y = startY;
    }
    int nearestberth_index = -1;
    int nearestgoods_index = -1;
};

/* 
    @brief: 在初始化中寻找机器人的位置
    @params: str: 地图每一行的首位指针
    @ret: 此行地图中机器人的纵坐标
*/
std::vector<int> GetRobotPlace(char* str);
/* 
    @brief: 根据给出的坐标移动机器人
    @params: robotid: 机器人编号; x, y: 机器人的当前位置; mhx, mhy: 机器人的下一帧位置
    @ret: none
*/
int robotmove(int (&robotmap)[n][n], int robotid, int x, int y, int mhx, int mhy);
int robotmove_nocheck();