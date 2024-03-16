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
    @params: robotmap, robotmap_next: 同 main.cpp; robotid: 机器人编号; x, y: 机器人当前位置;
             mhx, mhy: 机器人想去位置
    @ret: 0: 停一帧; 1: 移动成功; -1: 移动失败，main()中要重新算路线.
*/
int robotmove(int (&robotmap)[n][n], int (&robotmap_next)[n][n], int robotid, int x, int y, int mhx, int mhy)
{
    // 移动距离合法
    if((abs(x - mhx) + abs(y - mhy)) == 1)
    {
        // 新位置下一帧没有机器人，且对向没有机器人过来
        if(robotmap_next[mhx][mhy] == -1 && robotmap_next[x][y] == -1)
        {
            // 标记新位置
            robotmap_next[mhx][mhy] = robotid;
            /*这里发送移动指令*/
            robotmove_nocheck(robotid, x, y, mhx, mhy);
            return 1;
        }
        // 新位置下一帧有横向路过机器人，避让
        else if(robotmap_next[mhx][mhy] >= 0 && robotmap_next[x][y] == -1)
        {
            robotmap_next[x][y] = robotid;
            /*停一帧*/
            return 0;
        }
        // 新位置来了个疑似对撞的，原位置下一帧有机器人
        else if(robotmap_next[x][y] >= 0 && robotmap[mhx][mhy] >= 0)
        {
            // 还真是对撞，我闪
            if(robotmap_next[x][y] == robotmap[mhx][mhy])
            {
                return -1;
            }
            // 不是碰撞，真是惊险，继续走起
            else if(robotmap_next[mhx][mhy] == -1)
            {
                // 标记新位置
                robotmap_next[mhx][mhy] = robotid;
                /*这里发送移动指令*/
                robotmove_nocheck(robotid, x, y, mhx, mhy);
                return 1;
            }
            // 虽然不是碰撞，但是下一个位置已经被抢了
            else if(robotmap_next[mhx][mhy] >= 0)
            {
                return -1;
            }
        }
        // robotmap_next[x][y] >= 0 && robotmap[mhx][mhy] == -1
        // 虽然原位置要被占，但是新位置能去
        else {
            // 标记新位置
            robotmap_next[mhx][mhy] = robotid;
            /*这里发送移动指令*/
            robotmove_nocheck(robotid, x, y, mhx, mhy);
            return 1;
        }
    }
    return -1;
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