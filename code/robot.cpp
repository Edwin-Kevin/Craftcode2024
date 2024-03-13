#include "robot.h"

/* 
    @brief: 在初始化中寻找机器人的位置
    @params: str: 地图每一行的首位指针
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