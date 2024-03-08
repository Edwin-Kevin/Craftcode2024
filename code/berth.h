const int berth_num = 10;

/* 
    @brief: 泊位
    @params: x,y: 泊位坐标; transport_time: 泊位运输到虚拟点的时间（单位：帧，1~1000）;
             loading_speed: 每帧可装载物品数（1~5）
    @ret: none
*/
struct Berth
{
    int x;
    int y;
    int transport_time;
    int loading_speed;
    Berth(){}
    Berth(int x, int y, int transport_time, int loading_speed) {
        this -> x = x;
        this -> y = y;
        this -> transport_time = transport_time;
        this -> loading_speed = loading_speed;
    }
}berth[berth_num + 10];
