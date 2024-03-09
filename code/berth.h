const int berth_num = 10;

/* 
    @brief: 泊位
    @params: x,y: 泊位坐标; transport_time: 泊位运输到虚拟点的时间（单位：帧，1~1000）;
             loading_speed: 每帧可装载物品数（1~5）;
             weight: 结合装载时间和运输时间计算得到的加权函数;
             boat_index: 存储当前停靠船的编号，-1 代表泊位空闲。
    @ret: none
*/
struct Berth
{
    int x;
    int y;
    int transport_time;
    int loading_speed;
    int weight;
    int boat_index;
    Berth(){}
    Berth(int x, int y, int transport_time, int loading_speed) {
        this -> x = x;
        this -> y = y;
        this -> transport_time = transport_time;
        this -> loading_speed = loading_speed;
    }
}berth[berth_num + 10];
