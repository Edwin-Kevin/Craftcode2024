int boat_capacity;
/* 
    @brief: 船
    @params: num: 船只数量; pos: 目标泊位（虚拟点为 -1）;
             status: 状态（0 表示移动中，1 表示正常运行状态，2 表示泊位外等待状态）
    @ret: none
*/
struct Boat
{
    int num, pos, status;
}boat[10];

