#pragma once

/* 
    @brief: 货物
    @params: x,y: 货物坐标; status: 是否被拾取( 0 代表未被拾取， 1 代表已被拾取，2 代表运输完成);
             remaintime: 货物还能存活多久（单位：帧）;
             robotindex: 被分配给哪个机器人;
             val: 货物价值
*/
struct Goods
{
    int x, y;
    int status = 2;
    int remaintime = 0;
    int robotindex = -1;
    int val = 0;
};
