#define LOG  // Comment this line if no need for log file.
#include "main.h"

/* 
TODO: 1.如果跳帧，得修正计算船的装货数量和货物的剩余存留时长
2.机器人运动指令函数，里面包含碰撞机制(如果返回-1，需要重新astar然后再调用移动函数(while astar == -1))
 astar 也要改成考虑其他机器人下一帧位置，如果重新计算没结果，把 next_map 中原位置赋robotid
 碰撞不可避免，引入碰撞机制，机器人恢复状态清空对应paths，恢复后重算paths
3.把去港口路线上经过的点都标记为距离这个港口最近 
4.最近货物的选择：随机选两个直线距离最近的，然后计算实际距离
5.限制每一帧最多进行 10 次 A*
*/

char ch[n][n]; // 存储地图
bool availmap[n][n]; // 记录可达点的地图（包含陆地、机器人、港口位置）
int robotmap[n][n];  // 存储当前机器人位置的地图, 内容为机器人的编号
int robotmap_next[n][n]; // 存储下一帧机器人位置的地图, 内容为机器人的编号
int gds[n][n]; // 存储当前货物位置(内容为货物编号，-1 为无货)
int boat_capacity; // 船只容量
Berth berth[berth_num];
Robot robot[robot_num];
Goods goods[210];
// set 可以自动排序元素，使元素保持升序
set<int> minWeights;
// 使用 multimap 来存储权重值和对应的 berth 索引
multimap<int, int> weightToIndex;

std::vector<std::vector<std::pair<int, int>>> paths(10); // 10 个机器人的路径列表

#ifdef LOG
std::ofstream logFile("log.txt");
#endif

/*
    @brief: 为机器人分配一个最近的货物，同时计算路线存入 paths[robot_index](不含起点)
    @param: robot_index: 机器人的编号, range: 在此范围大小的邻域中查找货物(建议初始值为1)
    @ret: 货物编号
*/
int selectnearestGoods(int robot_index, int range);
int selectnearestGoods(int robot_index, int range)
{
    // 边界左上角
    int x_left = robot[robot_index].x - range, y_up = robot[robot_index].y - range;
    if(x_left < 0)  x_left = 0;
    if(y_up < 0)    y_up = 0;
    // 边界右下角
    int x_right = robot[robot_index].x + range, y_down = robot[robot_index].y + range;
    if(x_right >= n) x_right = n - 1;
    if(y_down >= n)  y_down = n - 1;

    for(int i = x_left; i <= x_right; ++i)
    {
        int j = y_up;
        // 有货且可达
        if(gds[i][j] > 0 && availmap[i][j] && goods[gds[i][j]].robotindex < 0)
        {
            paths[robot_index] = aStarSearch(ch, robot[robot_index].x, robot[robot_index].y, i, j);
            if(paths[robot_index].empty())
            {
                continue;
            }
            else{
                if(goods[gds[i][j]].remaintime > paths[robot_index].size())
                {
                    paths[robot_index].erase(paths[robot_index].begin());
                    goods[gds[i][j]].robotindex = robot_index;
                    return gds[i][j];
                }
            }
        }
        j = y_down;
        if(gds[i][j] > 0 && availmap[i][j] && goods[gds[i][j]].robotindex < 0)
        {
            paths[robot_index] = aStarSearch(ch, robot[robot_index].x, robot[robot_index].y, i, j);
            if(paths[robot_index].empty())
            {
                continue;
            }
            else{
                if(goods[gds[i][j]].remaintime > paths[robot_index].size())
                {
                    paths[robot_index].erase(paths[robot_index].begin());
                    goods[gds[i][j]].robotindex = robot_index;
                    return gds[i][j];
                }
            }
        }
    }

    for(int j = y_up; j <= y_down; j++)
    {
        int i = x_left;
        // 有货且可达
        if(gds[i][j] > 0 && availmap[i][j] && goods[gds[i][j]].robotindex < 0)
        {
            paths[robot_index] = aStarSearch(ch, robot[robot_index].x, robot[robot_index].y, i, j);
            if(paths[robot_index].empty())
            {
                continue;
            }
            else{
                if(goods[gds[i][j]].remaintime > paths[robot_index].size())
                {
                    paths[robot_index].erase(paths[robot_index].begin());
                    goods[gds[i][j]].robotindex = robot_index;
                    return gds[i][j];
                }
            }
        }
        i = x_right;
        // 有货且可达
        if(gds[i][j] > 0 && availmap[i][j] && goods[gds[i][j]].robotindex < 0)
        {
            paths[robot_index] = aStarSearch(ch, robot[robot_index].x, robot[robot_index].y, i, j);
            if(paths[robot_index].empty())
            {
                continue;
            }
            else{
                if(goods[gds[i][j]].remaintime > paths[robot_index].size())
                {
                    paths[robot_index].erase(paths[robot_index].begin());
                    goods[gds[i][j]].robotindex = robot_index;
                    return gds[i][j];
                }
            }
        }
    }
    // 扩大范围继续找
    return selectnearestGoods(robot_index, range + 1);
}

void Init()
{
#ifdef LOG
        logFile << "Init()" << endl;
#endif

    // 读入地图信息
    int robot_number = 0;
    for(int i = 0; i < n; i ++)
    {
        scanf("%s", ch[i]);
        // 从地图中读入机器人的初始位置
        std::vector<int> allPositions = GetRobotPlace(ch[i]);
        if (!allPositions.empty()) 
        {
            for (int pos : allPositions) 
            {
                //printf("%d ", pos);
                robot[robot_number].x = i;
                robot[robot_number].y = pos;
#ifdef LOG
                logFile << "robot[" << robot_number << "]: (" << robot[robot_number].x << ", " 
                << pos << ")" << endl;
#endif                    
                robot_number++;
            }
        }
    }
    initMap(ch, availmap);

    // 检查机器人是否在墙里
    for(int i = 0; i < robot_num; ++i)
    {
        int x = robot[i].x;
        int y = robot[i].y;
        if(availmap[x][y])  robot[i].enable = true;
    }

#ifdef LOG
    for(int i = 0; i < robot_num; ++i)
    {
        logFile << "Robot [" << i << "].enable: " << robot[i].enable << endl;
    }
    logFile << "Berth number: " << berth_num << endl;
#endif
    
    // 读入泊位信息
    for(int i = 0; i < berth_num; i ++)
    {
        int id;
        scanf("%d", &id);
        scanf("%d%d%d%d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed);
        // berth[i].boat_index = -1;
        // 计算该泊位的权重
        berth[i].weight = -200 * berth[i].loading_speed + 1 * berth[i].transport_time;
        // 将权重存入集合
        weightToIndex.insert(make_pair(berth[i].weight, i)); // 创建了一个包含权重和索引的键值对

#ifdef LOG
        logFile << "Berth[" << i << "]:" ;
        logFile << "Berth weight: " << berth[i].weight << endl;
#endif

        minWeights.insert(berth[i].weight); // 权重值存入 minWeights
        // 遍历 weightToIndex 集合，尝试更新 minWeights 集合
        for (const auto &pair : weightToIndex) {
            // 将当前 weight 添加到集合中
            // minWeights.insert(pair.first);

            // 如果集合中的元素多于五个，移除最大的元素
            if (minWeights.size() > 5) {
                minWeights.erase(--minWeights.end());
            }
        }
    }

#ifdef LOG
    logFile << "minWeights: " ;
    for (const auto& elem : minWeights) 
    {
        logFile << elem << " "; // 将集合中的每个元素写入文件
    }
    logFile << endl << "weightToIndex: " << endl;
    for (const auto& pair : weightToIndex) 
    {
        logFile << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    }
#endif

    // 初始化货物地图
    for(int i = 0; i < n; ++i)
    {
        for(int j = 0; j < n; ++j)
        {
            gds[i][j] = -1;
        }
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
        for(int j = 0; j < 210; j++)
        {
            // 消失的货物和运输完成的货物可以被释放
            if(goods[j].remaintime <= 0 || goods[j].status == 2)
            {
                goods[j].remaintime = 1000;
                goods[j].x = x;
                goods[j].y = y;
                goods[j].val = val;
                goods[j].status = 0;
                goods[j].robotindex = -1;
                gds[x][y] = j;
                break;
            }
        }
    }

    // 初始化机器人分布图
    for(int i = 0; i < n; ++i)
    {
        for(int j = 0; j < n; ++j)
        {
            robotmap[i][j] = -1;
            robotmap_next[i][j] = -1;
            if(ch[i][j] == 'A')
            {
                ch[i][j] = '.';
            }
        }
    }
    // 获取机器人信息
    for(int i = 0; i < robot_num; i ++)
    {
        int sts;
        // 分别为：是否携带货物、机器人坐标、机器人状态（0 表示恢复状态，1 表示正常运行状态）
        scanf("%d%d%d%d", &robot[i].goods, &robot[i].x, &robot[i].y, &robot[i].status);
        int x = robot[i].x;
        int y = robot[i].y;
        robotmap[x][y] = i;
        ch[x][y] = 'A';
        robot[i].actioned = false;

        if(robot[i].goods == 1)
        {
            // 将货物标记为已被拾取
            goods[robot[i].nearestgoods_index].status = 1;
            // 将货物从地图上删去
            gds[goods[robot[i].nearestgoods_index].x][goods[robot[i].nearestgoods_index].y] = -1;
        }
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
    for(int frame = 1; frame <= 15000; ++ frame)
    {
        int id = Input();
#ifdef LOG
        logFile << "Frame " << frame << std::endl;
#endif
//---------------------------------------BERTH---------------------------------------//
        // 输出最小的五个 weight 以及它们对应的 berth 索引，从而选择五个最好的港口
        int cnt = 0; // 轮船编号
        for (int weight : minWeights) {
#ifdef LOG
            logFile << "boatcnt: " << cnt << " weight: " << weight << endl;
#endif
            auto range = weightToIndex.equal_range(weight); // 在 weightToIndex 中查找 weight 值
            // range.first 和 range.second 分别是迭代器，指向匹配的第一个元素和超出匹配元素序列的元素
            for (auto it = range.first; it != range.second; ++it) {
                int berthIndex = it -> second;
                // 船只在虚拟点且可移动
                if(boat[cnt].status == BOAT_STATUS_NORMAL && boat[cnt].pos == -1)
                {
                    berth[berthIndex].boat_index = cnt;
                    printf("ship %d %d\n", cnt, berthIndex);
#ifdef LOG
                    logFile << "ship " << cnt << " " << berthIndex << std::endl;
#endif
                }
                ++cnt;
            }
            
        }

//-----------------------------------ROBOT------------------------------------------------//
        // 测试阶段只开一个机器人
        // 本轮执行动作的机器人数量，如果为0就可以退出循环了
        while(true) {
            int actioned_bot = 0;
            for (int robotcnt = 0; robotcnt < 2; robotcnt++) {
                if (robot[robotcnt].enable && !robot[robotcnt].actioned) {
                    // 机器人闲着
                    if (paths[robotcnt].empty()) {
                        // 机器人运货到泊位了
                        if (robot[robotcnt].x == berth[robot[robotcnt].nearestberth_index].x &&
                            robot[robotcnt].y == berth[robot[robotcnt].nearestberth_index].y &&
                            robot[robotcnt].goods == 1) {
                            printf("pull %d\n", robotcnt);
                            actioned_bot++;
                            robot[robotcnt].actioned = true;
                            goods[robot[robotcnt].nearestgoods_index].status = 2;
                            goods[robot[robotcnt].nearestgoods_index].robotindex = -1;
                            robot[robotcnt].nearestgoods_index = -1;
                            robot[robotcnt].goods = 0;
#ifdef LOG
                            logFile << "Robot " << robotcnt << " reached the berth! " << endl;
                            logFile << "pull " << robotcnt << endl;
#endif
                        }
                            // 机器人到取货点了
                        else if (robot[robotcnt].x == goods[robot[robotcnt].nearestgoods_index].x &&
                                 robot[robotcnt].y == goods[robot[robotcnt].nearestgoods_index].y &&
                                 robot[robotcnt].goods == 0 &&
                                 goods[robot[robotcnt].nearestgoods_index].remaintime > 0 &&
                                 goods[robot[robotcnt].nearestgoods_index].status == 0) {
                            printf("get %d\n", robotcnt);
                            actioned_bot++;
                            robot[robotcnt].actioned = true;
#ifdef LOG
                            logFile << "Robot " << robotcnt << " reached the goods! " << endl;
                            logFile << "robot[" << robotcnt << "]: (" << robot[robotcnt].x << ", " << robot[robotcnt].y
                                    << ")" << endl;
                            logFile << "goods: (" << goods[robot[robotcnt].nearestgoods_index].x;
                            logFile << ", " << goods[robot[robotcnt].nearestgoods_index].y << ") ";
                            logFile << "remaintime: " << goods[robot[robotcnt].nearestgoods_index].remaintime << endl;
                            logFile << "get " << robotcnt << endl;
#endif
                        }
                            // 机器人空闲，分配新任务
                        else {
                            if (robot[robotcnt].goods == 1) {
                                // 带了货物，去泊位
                                robot[robotcnt].nearestberth_index = nearest_berth(robot[robotcnt].x,
                                                                                   robot[robotcnt].y);
                                paths[robotcnt] = aStarSearch(ch, robot[robotcnt].x, robot[robotcnt].y,
                                                              berth[robot[robotcnt].nearestberth_index].x,
                                                              berth[robot[robotcnt].nearestberth_index].y);
                                // 路径去掉起点
                                paths[robotcnt].erase(paths[robotcnt].begin());
                            } else if (robot[robotcnt].goods == 0) {
                                // 没带货物，去找货物
                                robot[robotcnt].nearestgoods_index = selectnearestGoods(robotcnt, 1);
#ifdef LOG
                                logFile << "nearestgoods_index: " << robot[robotcnt].nearestgoods_index << endl;
                                if (paths[robotcnt].size() != 0) {
                                    logFile << "goods(x, y): " << "(" << goods[robot[robotcnt].nearestgoods_index].x
                                            << ", " << goods[robot[robotcnt].nearestgoods_index].y << ")" << endl;
                                }
#endif
                            }
                        }
                    }
                    // 路径列表非空
                    if (!paths[robotcnt].empty()) {
                        // 取出下一个坐标点
                        pair<int, int> next_step = paths[robotcnt].front();
#ifdef LOG
                        logFile << "Robot " << robotcnt << " Next step: (" << next_step.first << ", "
                                << next_step.second << ")" << endl;
#endif
                        int move_result;
                        move_result = robotmove(ch, robotcnt, robot[robotcnt].x, robot[robotcnt].y, next_step.first,
                                                next_step.second);
                        // 下一步移动失败
                        if (move_result == 0)
                            // 移动失败，要重新算
                        {
#ifdef LOG
                            logFile << "Robot move failed." << endl;
#endif
                            paths[robotcnt].clear();
                        }
                        if (move_result == 1) {
                            // 移动成功，并且没到目的地，机器人操作结束
                            paths[robotcnt].erase(paths[robotcnt].begin());
                            if (!paths[robotcnt].empty()) {
                                robot[robotcnt].actioned = true;
                            }
                            robot[robotcnt].x = next_step.first;
                            robot[robotcnt].y = next_step.second;
                            actioned_bot++;
#ifdef LOG
                            logFile << "Robot " << robotcnt << " moved successful!" << endl;
#endif
                        }
                    }
                }
            }
            if(actioned_bot == 0)
                break;
        }
//-----------------------------------BOAT------------------------------------//
        // 检查船只
        for(int i = 0; i < boat_num; ++ i)
        {
            // 如果船装满了货，就开走
            if(boat[i].goods >= boat_capacity && boat[i].status == BOAT_STATUS_NORMAL)
            {
                berth[boat[i].pos].boat_index = -1;
                printf("go %d\n", i);
            }
        }
        if(frame == 13000)
        {
            for(int i = 0; i < boat_num; i++)
            {
                printf("go %d\n", i);
            }
        }

//----------------------------------GOODS-----------------------------------//
        // 更新每一个货物的存活时间
        for(int i = 0; i < 210; i++)
        {
            if(goods[i].remaintime > 0 && goods[i].status == 0)
            {
                goods[i].remaintime --;
                if(goods[i].remaintime <= 0)
                {
                    goods[i].status = 2;
                    gds[goods[i].x][goods[i].y] = -1;
                }
            }
        }

//----------------------------------BERTH----------------------------------//
        // 检查港口
        for(int i = 0; i < berth_num; ++i)
        {
            // 港口将货物装载到船上
            if(berth[i].goods > 0 && berth[i].boat_index != -1 && boat[berth[i].boat_index].status == BOAT_STATUS_NORMAL)
            {
                // 如果船没装满
                if(boat[berth[i].boat_index].goods < boat_capacity)
                {
                    // 如果港口堆积的货物比 loading_speed 要多，并且船剩下的容量也大于 loading_speed
                    if((berth[i].goods > berth[i].loading_speed) &&
                     ((boat_capacity - boat[berth[i].boat_index].goods) > berth[i].loading_speed))
                    {
                        // 那么就装 loading_speed 个货
                        boat[berth[i].boat_index].goods += berth[i].loading_speed;
                        berth[i].goods -= berth[i].loading_speed;
                    }
                    else{
                        // 否则把港口的货全装上，或者把船装满，两者满足其一
                        boat[berth[i].boat_index].goods += min(berth[i].goods, boat_capacity - boat[berth[i].boat_index].goods);
                        berth[i].goods -= min(berth[i].goods, boat_capacity - boat[berth[i].boat_index].goods);
                    }
                }
            }
        }
        puts("OK");
        fflush(stdout);
    }
#ifdef LOG
    logFile.close();
#endif
    return 0;
}