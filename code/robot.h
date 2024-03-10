#include <cstdio>
#include <cstring>
#include <vector>

#define ROBOT_MOVE_RIGHT  0
#define ROBOT_MOVE_LEFT   1
#define ROBOT_MOVE_UP     2
#define ROBOT_MOVE_DOWN   3

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

/* 
    @brief: 在初始化中寻找机器人的位置
    @params: str: 
    @ret: std::vector<int>
*/
std::vector<int> GetRobotPlace(char* str) 
{
    std::vector<int> positions; // 用于存储字符 'A' 的位置
    char* position = str;

    // 使用循环查找字符串中的所有字符 'A'
    while ((position = strchr(position, 'A')) != nullptr) 
    {
        int pos = position - str;
        positions.push_back(pos);
        position++; // 移动到下一个位置继续搜索
    }
    return positions;
}