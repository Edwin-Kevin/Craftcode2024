#include <bits/stdc++.h>
#include "berth.h"

/* 
    @brief: 寻找最近泊位
    @params: x,y: 机器人的横坐标和纵坐标
    @ret: 最近泊位的编号
*/
int nearest_port(int x, int y)
{
    extern struct Berth berth[berth_num];
    
    int nearestid;
    int min = 400;
    for(int i = 0; i < berth_num; ++i)
    {
        if(abs(berth[i].x - x) + abs(berth[i].y - y) < min)
        {
            min = abs(berth[i].x - x) + abs(berth[i].y - y);
            nearestid = i;
        };
    }
    return nearestid;
}
