const int robot_num = 10;

/* 
    @brief: 机器人
    @params: x,y: 机器人坐标; goods: 是否携带物品;
             status: 状态（0 表示恢复状态，1 表示正常运行状态）
    @ret: none
*/
struct Robot
{
    int x, y, goods;
    int status;
    int mbx, mby;
    Robot() {}
    Robot(int startX, int startY) {
        x = startX;
        y = startY;
    }
}robot[robot_num + 10];

