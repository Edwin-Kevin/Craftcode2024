#pragma once

#define BOAT_STATUS_MOVING 0
#define BOAT_STATUS_NORMAL 1
#define BOAT_STATUS_WAITING 2

const int boat_num = 5;
int boat_capacity;
/* 
    @brief: 船
    @params: pos: 目标泊位（虚拟点为 -1）;
             status: 状态（0 表示移动中，1 表示正常运行状态，2 表示泊位外等待状态）;
             goods: 已装载货物数量，不得大于 boat_capacity.
    @ret: none
*/
struct Boat
{
    int pos, status;
    int goods = 0;
}boat[boat_num];
