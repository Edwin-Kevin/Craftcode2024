#define LOG  // Comment this line if no need for log file.
#include "main.h"

char ch[n][n]; // 存储地图
bool availmap[n][n]; // 记录可达点的地图（包含陆地、机器人、港口位置）
int robotmap[n][n];  // 存储机器人位置的地图, 0 为空, 1 为下一帧有机器人, 2 为当前有机器人，每帧更新
bool roadmap[n][n]; // 给寻路算法使用的路径规划图(每帧结合 availmap 和 robotmap 计算)
// int gds[n][n]; // 存储当前货物位置
int boat_capacity;
Berth berth[berth_num];
Robot robot[robot_num];
Goods goods[100];
// set 可以自动排序元素，使元素保持升序
set<int> minWeights;
// 使用 multimap 来存储权重值和对应的 berth 索引
multimap<int, int> weightToIndex;

std::vector<std::vector<std::pair<int, int>>> paths(10); // 10 个机器人的路径列表

#ifdef LOG
std::ofstream logFile("log.txt");
#endif

//---------------------------------------函数定义----------------------------------------------//
/*
    @brief: 为机器人分配一个货物
    @param: robot_index: 机器人的编号, good_begin_index: 从这个索引开始找
    @ret: 货物编号
*/
int selectGoods(int robot_index, int good_begin_index);
int selectGoods(int robot_index, int good_begin_index)
{
    for(int i = good_begin_index; i < 100; i++)
    {
        if(goods[i].status == 0 && goods[i].robotindex < 0 && goods[i].remaintime > 0)
        {
            int x, y;
            x = goods[i].x;
            y = goods[i].y;
            if(!availmap[x][y])
            {
                continue;
            }
            goods[i].robotindex = robot_index;
            return i;
        }
    }
    return -1;
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
        for(int j = 0; j < 100; j++)
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
                break;
            }
        }
    }

    // 初始化机器人分布图
    for(int i = 0; i < n; ++i)
    {
        for(int j = 0; j < n; ++j)
        {
            robotmap[i][j] = 0;
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
        robotmap[x][y] = 1;

        if(robot[i].goods == 1)
        {
            // 将货物标记为已被拾取
            goods[robot[i].nearestgoods_index].status = 1;
        }
        if(!paths[i].empty())
        {
            if(robot[i].mbx != robot[i].x || robot[i].mby != robot[i].y)
            {
                // 说明机器人位置计算错了，要重算路线
                paths[i].clear();
            }
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
        // if(frame == 1)
        // {
        //     for(int i = 0; i < n; ++i)
        //     {
        //         for(int j = 0; j < n; ++j)
        //         {
        //             logFile << robotmap[i][j] << " ";
        //         }
        //         logFile << endl;
        //     }
        // }
#endif
        // 输出对机器人的操作指令
        // for(int i = 0; i < robot_num; ++ i)
        // {
        //     printf("move %d %d\n", i, rand() % 4);
        // }

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
        for(int robotcnt = 0; robotcnt < 1; robotcnt++)
        {
            if(robot[robotcnt].enable)
            {
                // 机器人闲着
                if(paths[robotcnt].empty())
                {
                    // 机器人运货到泊位了
                    if(robot[robotcnt].x == berth[robot[robotcnt].nearestberth_index].x && 
                        robot[robotcnt].y == berth[robot[robotcnt].nearestberth_index].y && robot[robotcnt].goods == 1)
                    {
                        printf("pull %d\n", robotcnt);
                        goods[robot[robotcnt].nearestgoods_index].status = 2;
                        goods[robot[robotcnt].nearestgoods_index].robotindex = -1;
                        robot[robotcnt].nearestgoods_index = -1;
                        robot[robotcnt].goods = 0;
#ifdef LOG
                        logFile << "Robot " << robotcnt<< " reached the berth! " << endl;
                        logFile << "pull "<< robotcnt << endl;
#endif
                    }
                    // 机器人到取货点了
                    else if(robot[robotcnt].x == goods[robot[robotcnt].nearestgoods_index].x && 
                        robot[robotcnt].y == goods[robot[robotcnt].nearestgoods_index].y && robot[robotcnt].goods == 0 && 
                        goods[robot[robotcnt].nearestgoods_index].remaintime > 0)
                    {
                        printf("get %d\n", robotcnt);
#ifdef LOG
                        logFile << "Robot " << robotcnt << " reached the goods! " << endl;
                        logFile << "robot[" << robotcnt << "]: (" << robot[robotcnt].x << ", " << robot[robotcnt].y << ")" << endl;
                        logFile << "goods: (" << goods[robot[robotcnt].nearestgoods_index].x;
                        logFile << ", " << goods[robot[robotcnt].nearestgoods_index].y << ") ";
                        logFile << "remaintime: " << goods[robot[robotcnt].nearestgoods_index].remaintime << endl;
                        logFile << "get " << robotcnt << endl;
#endif
                    }
                    // 机器人空闲，分配新任务
                    else{
                        if(robot[robotcnt].goods == 1)
                        {
                            // 带了货物，去泊位
                            robot[robotcnt].nearestberth_index = nearest_berth(robot[robotcnt].x, robot[robotcnt].y);
                            paths[robotcnt] = aStarSearch(ch, robot[robotcnt].x, robot[robotcnt].y,
                                berth[robot[robotcnt].nearestberth_index].x, berth[robot[robotcnt].nearestberth_index].y);
                            // 路径去掉起点
                            paths[robotcnt].erase(paths[robotcnt].begin());
                        }
                        else if(robot[robotcnt].goods == 0)
                        {
                            // 没带货物，去找货物
                            robot[robotcnt].nearestgoods_index = selectGoods(robotcnt, robot[robotcnt].nearestgoods_index + 1);
                            paths[robotcnt] = aStarSearch(ch, robot[robotcnt].x, robot[robotcnt].y,
                                goods[robot[robotcnt].nearestgoods_index].x, goods[robot[robotcnt].nearestgoods_index].y);
                            // 来不及去了，或者货物刷墙里了，那这个机器人就休息一帧
                            // 每次最多搜索三次
                            int searchcnt = 0;
                            // 来不及去了，或者货物刷墙里了
                            while(((paths[robotcnt].size() > (goods[robot[robotcnt].nearestgoods_index].remaintime)) || paths[robotcnt].size() == 0) && searchcnt < 3)
                            {
#ifdef LOG
                                // logFile << "paths[robotcnt].size(): " << paths[robotcnt].size() << endl;
#endif
                                paths[robotcnt].clear();
                                if(robot[robotcnt].nearestgoods_index > 97)
                                {
                                    robot[robotcnt].nearestgoods_index = 0;
                                }
                                else{
                                    robot[robotcnt].nearestgoods_index ++;
                                }
                                robot[robotcnt].nearestgoods_index = selectGoods(robotcnt, robot[robotcnt].nearestgoods_index + 1);
                                paths[robotcnt] = aStarSearch(ch, robot[robotcnt].x, robot[robotcnt].y,
                                    goods[robot[robotcnt].nearestgoods_index].x, goods[robot[robotcnt].nearestgoods_index].y);
                                searchcnt++;
                            }
                            if(!paths[robotcnt].empty())
                            {
                                // 把起点去掉
                                paths[robotcnt].erase(paths[robotcnt].begin());
                            }
#ifdef LOG
                            logFile << "nearestgoods_index: " << robot[robotcnt].nearestgoods_index << endl;
                            if(paths[robotcnt].size() != 0)
                            {
                                logFile << "goods(x, y): " << "(" << goods[robot[robotcnt].nearestgoods_index].x << ", " << goods[robot[robotcnt].nearestgoods_index].y << ")" << endl;
                                for(const auto& p : paths[robotcnt])
                                {
                                    logFile << "(" << p.first << ", " << p.second << ") ";
                                }
                                logFile << endl;
                            }
#endif
                        }
                    }
                }
                // 路径列表非空
                if(!paths[robotcnt].empty())
                {
                    // 取出下一个坐标点
                    pair<int, int> next_step = paths[robotcnt].front();
                    paths[robotcnt].erase(paths[robotcnt].begin());
#ifdef LOG
                    logFile << "Robot " << robotcnt << " Next step: (" << next_step.first << ", " << next_step.second << ")" << endl;
                    logFile << "next_step.first - robot[" << robotcnt << "].x : " << next_step.first - robot[robotcnt].x << endl;
                    logFile << "next_step.second - robot[" << robotcnt << "].y : " << next_step.second - robot[robotcnt].y << endl;
#endif
                    // 下一步的规划合理
                    if((abs(next_step.first - robot[robotcnt].x) + abs(next_step.second - robot[robotcnt].y)) == 1)
                    {
                        robot[robotcnt].mbx = next_step.first;
                        robot[robotcnt].mby = next_step.second;
                        if((next_step.first - robot[robotcnt].x) == 1)
                        {
                            printf("move %d %d\n", robotcnt, ROBOT_MOVE_DOWN);
#ifdef LOG
                            logFile << "move " << robotcnt << " " << ROBOT_MOVE_DOWN << endl;
#endif
                        }
                        else if((next_step.first - robot[robotcnt].x) == -1)
                        {
                            printf("move %d %d\n", robotcnt, ROBOT_MOVE_UP);
#ifdef LOG
                            logFile << "move " << robotcnt << " " << ROBOT_MOVE_UP << endl;
#endif
                        }
                        else if((next_step.second - robot[robotcnt].y) == 1)
                        {
                            printf("move %d %d\n", robotcnt, ROBOT_MOVE_RIGHT);
#ifdef LOG
                            logFile << "move " << robotcnt << " " << ROBOT_MOVE_RIGHT << endl;
#endif
                        }
                        else if((next_step.second - robot[robotcnt].y) == -1)
                        {
                            printf("move %d %d\n", robotcnt, ROBOT_MOVE_LEFT);
#ifdef LOG
                            logFile << "move " << robotcnt << " " << ROBOT_MOVE_LEFT << endl;
#endif
                        }
                    }
                    else{
                        // 路线规划不合理，重新计算
                        if(robot[robotcnt].goods == 1)
                        {
                            // 带了货物，去泊位
                            robot[robotcnt].nearestberth_index = nearest_berth(robot[robotcnt].x, robot[robotcnt].y);
                            paths[robotcnt] = aStarSearch(ch, robot[robotcnt].x, robot[robotcnt].y,
                                berth[robot[robotcnt].nearestberth_index].x, berth[robot[robotcnt].nearestberth_index].y);
                            // 路径去掉起点
                            paths[robotcnt].erase(paths[robotcnt].begin());
                        }
                        else if(robot[robotcnt].goods == 0)
                        {
                            // 没带货物，去找货物
                            robot[robotcnt].nearestgoods_index = selectGoods(robotcnt, robot[robotcnt].nearestgoods_index + 1);
                            paths[robotcnt] = aStarSearch(ch, robot[robotcnt].x, robot[robotcnt].y,
                                goods[robot[robotcnt].nearestgoods_index].x, goods[robot[robotcnt].nearestgoods_index].y);
                            
                            // 每次最多搜索三次
                            int searchcnt = 0;
                            // 来不及去了，或者货物刷墙里了
                            while(((paths[robotcnt].size() > (goods[robot[robotcnt].nearestgoods_index].remaintime)) || paths[robotcnt].size() == 0) && searchcnt < 3)
                            {
                                paths[robotcnt].clear();
                                if(robot[robotcnt].nearestgoods_index > 97)
                                {
                                    robot[robotcnt].nearestgoods_index = 0;
                                }
                                else{
                                    robot[robotcnt].nearestgoods_index ++;
                                }
                                robot[robotcnt].nearestgoods_index = selectGoods(robotcnt, robot[robotcnt].nearestgoods_index + 1);
                                paths[robotcnt] = aStarSearch(ch, robot[robotcnt].x, robot[robotcnt].y,
                                    goods[robot[robotcnt].nearestgoods_index].x, goods[robot[robotcnt].nearestgoods_index].y);
                                searchcnt++;
                            }
                            if(!paths[robotcnt].empty())
                            {
                                paths[robotcnt].erase(paths[robotcnt].begin());
                            }
                        }
                    }
                }
            }
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

//----------------------------------GOODS-----------------------------------//
        // 更新每一个货物的存活时间
        for(int i = 0; i < 100; i++)
        {
            if(goods[i].remaintime > 0 && goods[i].status == 0)
            {
                goods[i].remaintime --;
                if(goods[i].remaintime <= 0)
                {
                    goods[i].status = 2;
                }
            }
        }

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