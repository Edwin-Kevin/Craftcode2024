#include <bits/stdc++.h>
using namespace std;

const int n = 200;
const int robot_num = 10;
const int berth_num = 10;
const int N = 210;

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

int money, boat_capacity, id;
char ch[N][N];
int gds[N][N];
void Init()
{
    // 读入地图信息
    for(int i = 1; i <= n; i ++)
        scanf("%s", ch[i] + 1);
    
    // 读入泊位信息
    for(int i = 0; i < berth_num; i ++)
    {
        int id;
        scanf("%d", &id);
        scanf("%d%d%d%d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed);
    }
    scanf("%d", &boat_capacity);
    char okk[100];
    scanf("%s", okk);
    
    // 初始化完成
    printf("OK\n");
    fflush(stdout);
}

int Input()
{
    scanf("%d%d", &id, &money); // 获取帧序号和当前金钱
    int num; // 获取场上新增货物的数量
    scanf("%d", &num);

    // 获取货物信息
    for(int i = 1; i <= num; i ++)
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val);
    }

    // 获取机器人信息
    for(int i = 0; i < robot_num; i ++)
    {
        int sts;
        // 分别为：是否携带货物、机器人坐标、机器人状态（0 表示恢复状态，1 表示正常运行状态）
        scanf("%d%d%d%d", &robot[i].goods, &robot[i].x, &robot[i].y, &sts);
    }
    
    // 获取船信息
    for(int i = 0; i < 5; i ++)
        // 船的状态（0 表示移动中，1 表示正常运行状态，2 表示泊位外等待状态）、
        // 目标泊位（虚拟点为 -1）
        scanf("%d%d\n", &boat[i].status, &boat[i].pos);
    char okk[100];
    scanf("%s", okk);
    return id;
}

int main()
{
    Init();
    // ++ 放变量前面，执行效率更高
    for(int zhen = 1; zhen <= 15000; ++ zhen)
    {
        int id = Input();
        // 输出对机器人和船的操作指令
        for(int i = 0; i < robot_num; ++ i)
            printf("move %d %d\n", i, rand() % 4);
        puts("OK");
        fflush(stdout);
    }

    return 0;
}
