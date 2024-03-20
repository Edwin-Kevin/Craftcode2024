#pragma once

#include <bits/stdc++.h>

#ifndef MAP_H

#define MAP_H
#define N 210
#define n 200
extern char ch[][n];
extern int gds[][n];

/* 
    @brief: 在初始化中查找地图可达点
    @params: ch: 给入的地图; outputMap: 输出的可达性地图(不可达为 false)
    @ret: std::vector<int>
*/
void initMap(char ch[n][n], int outputMap[n][n]);

#endif