#include <bits/stdc++.h>
#include "berth.h"

/* 
    @brief: 寻找最近泊位
    @params: x,y: 机器人的横坐标和纵坐标
    @ret: 最近泊位的编号
*/
int nearest_berth(int availmap[n][n], int x, int y)
{
    extern struct Berth berth[berth_num];

    int nearestid;
    int min = 400;
    for(int i = 0; i < berth_num; i++)
    {
        if(((abs(berth[i].x - x) + abs(berth[i].y - y)) < min) && berth[i].boat_index >= 0
        && availmap[berth[i].x][berth[i].y] == availmap[x][y])
        {
            min = abs(berth[i].x - x) + abs(berth[i].y - y);
            nearestid = i;
        };
    }
    return nearestid;
}

int second_nearest_berth(int availmap[n][n], int x, int y)
{
    extern struct Berth berth[berth_num];

    int nearestid;
    int second_nearestid;
    int min = 400;
    int sec_min = 400;
    for(int i = 0; i < berth_num; i++)
    {
        if(((abs(berth[i].x - x) + abs(berth[i].y - y)) < min) && berth[i].boat_index >= 0
        && availmap[berth[i].x][berth[i].y] == availmap[x][y])
        {
            min = abs(berth[i].x - x) + abs(berth[i].y - y);
            nearestid = i;
        };
    }
    for(int i = 0; i < berth_num; i++)
    {
        if(((abs(berth[i].x - x) + abs(berth[i].y - y)) > min) &&
            ((abs(berth[i].x - x) + abs(berth[i].y - y)) < sec_min) && berth[i].boat_index >= 0
                && availmap[berth[i].x][berth[i].y] == availmap[x][y])
        {
            sec_min = abs(berth[i].x - x) + abs(berth[i].y - y);
            second_nearestid = i;
        };
    }
    return second_nearestid;

}
