#include "robot.h"

/* 
    @brief: 在初始化中寻找机器人的位置
    @params: str: 地图每一行的首位指针
    @ret: 此行地图中机器人的纵坐标
*/
std::vector<int> GetRobotPlace(char* str) 
{
    std::vector<int> positions; // 用于存储字符 'A' 的位置
    char* position = str;

    // 使用循环查找字符串中的所有字符 'A'
    while ((position = strchr(position, 'A')) != nullptr) 
    {
        int pos = position - str;
        positions.push_back(pos);
        position++; // 移动到下一个位置继续搜索
    }
    return positions;
}

/*
    @brief: 检查后移动机器人
    @params: ch: 同 main.cpp; robotid: 机器人编号; x, y: 机器人当前位置;
             mhx, mhy: 机器人想去位置
    @ret: 0: 未移动; 1: 移动成功.
*/
int robotmove(char (&ch)[n][n], int robotid, int x, int y, int mhx, int mhy)
{
    if(ch[mhx][mhy] == '.' || ch[mhx][mhy] == 'B')
    {
        ch[x][y] = '.';
        ch[mhx][mhy] = 'A';
        robotmove_nocheck(robotid, x, y, mhx, mhy);
        return 1;
    }
    else{
        return 0;
    }
}

/*
    @brief: 根据给出的坐标移动机器人(没有校验)
    @params: robotid: 机器人编号; x, y: 机器人的当前位置; mhx, mhy: 机器人的下一帧位置
    @ret: none
*/
void robotmove_nocheck(int robotid, int x, int y, int mhx, int mhy)
{
    int dx = mhx - x;
    int dy = mhy - y;
    if(dx == -1)
    {
        printf("move %d %d\n", robotid, ROBOT_MOVE_UP);
    }
    else if(dx == 1)
    {
        printf("move %d %d\n", robotid, ROBOT_MOVE_DOWN);
    }
    else if(dy == -1)
    {
        printf("move %d %d\n", robotid, ROBOT_MOVE_LEFT);
    }
    else if(dy == 1)
    {
        printf("move %d %d\n", robotid, ROBOT_MOVE_RIGHT);
    }
}