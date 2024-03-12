#define LOG  // Comment this line if no need for log file.
#include "main.h"

char ch[n][n]; // 存储地图
int gds[n][n]; // 存储当前货物位置
int boat_capacity;
Berth berth[berth_num];
Robot robot[robot_num];
Goods goods[100];
// set 可以自动排序元素，使元素保持升序
set<int> minWeights;
// 使用 multimap 来存储权重值和对应的 berth 索引
multimap<int, int> weightToIndex;

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

#ifdef LOG
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
                break;
            }
        }
    }

    // 获取机器人信息
    for(int i = 0; i < robot_num; i ++)
    {
        int sts;
        // 分别为：是否携带货物、机器人坐标、机器人状态（0 表示恢复状态，1 表示正常运行状态）
        scanf("%d%d%d%d", &robot[i].goods, &robot[i].x, &robot[i].y, &sts);
        if(robot[i].goods == 1)
        {
            // 将货物标记为已被拾取
            goods[robot[i].nearestgoods_index].status = 1;
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
    std::vector<std::vector<std::pair<int, int>>> paths(10); // 10 个机器人的路径列表
    for(int frame = 1; frame <= 15000; ++ frame)
    {
#ifdef LOG
        logFile << "Frame " << frame << std::endl;
#endif
        int id = Input();
        // 输出对机器人的操作指令
        // for(int i = 0; i < robot_num; ++ i)
        // {
        //     printf("move %d %d\n", i, rand() % 4);
        // }

//---------------------------------------BERTH---------------------------------------//
        // 输出最小的五个 weight 以及它们对应的 berth 索引，从而选择五个最好的港口
        int cnt = 0; // 轮船编号
        for (int weight : minWeights) {
// #ifdef LOG
//             logFile << "boatcnt: " << cnt << " weight: " << weight << endl;
// #endif
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
        // 检查路径列表是否为空
        if(!paths[0].empty())
        {
#ifdef LOG
            logFile << "Line 222 hit." << endl;
#endif
            // 以下此句有问题
            pair<int, int> next_step = paths[0].front();
#ifdef LOG
            logFile << "Line 227: " << next_step.first << ", " << next_step.second << endl;
#endif
            paths[0].erase(paths[0].begin());
#ifdef LOG
            logFile << "Robot 0 Next step: (" << next_step.first << ", " << next_step.second << ")" << endl;
            logFile << "next_step.first - robot[0].x : " << next_step.first - robot[0].x << endl;
            logFile << "next_step.second - robot[0].y : " << next_step.second - robot[0].y << endl;
#endif
            if((abs(next_step.first - robot[0].x) + abs(next_step.second - robot[0].y)) == 1)
            {
                if((next_step.first - robot[0].x) == 1)
                {
                    printf("move %d %d\n", 0, ROBOT_MOVE_DOWN);
#ifdef LOG
                    logFile << "move 0 " << ROBOT_MOVE_DOWN << endl;
#endif
                }
                else if((next_step.first - robot[0].x) == -1)
                {
                    printf("move %d %d\n", 0, ROBOT_MOVE_UP);
#ifdef LOG
                    logFile << "move 0 " << ROBOT_MOVE_UP << endl;
#endif
                }
                else if((next_step.second - robot[0].y) == 1)
                {
                    printf("move %d %d\n", 0, ROBOT_MOVE_RIGHT);
#ifdef LOG
                    logFile << "move 0 " << ROBOT_MOVE_RIGHT << endl;
#endif
                }
                else if((next_step.second - robot[0].y) == -1)
                {
                    printf("move %d %d\n", 0, ROBOT_MOVE_LEFT);
#ifdef LOG
                    logFile << "move 0 " << ROBOT_MOVE_LEFT << endl;
#endif
                }
            }
            else{
                // 路线规划有误，重新计算
                if(robot[0].goods == 1)
                {
                    // 带了货物，去泊位
                    robot[0].nearestberth_index = nearest_berth(robot[0].x, robot[0].y);
                    paths[0] = aStarSearch(ch, robot[0].x, robot[0].y,
                        berth[robot[0].nearestberth_index].x, berth[robot[0].nearestberth_index].y);
                    // 路径去掉起点
                    paths[0].erase(paths[0].begin());
                }
                else if(robot[0].goods == 0)
                {
                    // 没带货物，去找货物
                    robot[0].nearestgoods_index = selectGoods(0, robot[0].nearestgoods_index + 1);
                    paths[0] = aStarSearch(ch, robot[0].x, robot[0].y,
                        goods[robot[0].nearestgoods_index].x, goods[robot[0].nearestgoods_index].y);
                    // 来不及去了，或者货物刷墙里了
                    if((paths[0].size() > (goods[robot[0].nearestgoods_index].remaintime)) || paths[0].size() == 0)
                    {
                        paths[0].clear();
                        robot[0].nearestgoods_index ++;
                        // robot[0].nearestgoods_index = selectGoods(0, robot[0].nearestgoods_index + 1);
                        // paths[0] = aStarSearch(ch, robot[0].x, robot[0].y,
                        //     goods[robot[0].nearestgoods_index].x, goods[robot[0].nearestgoods_index].y);
                    }
                    else{
                        paths[0].erase(paths[0].begin());
                    }
                }
            }
        }
        // 机器人运货到泊位了
        else if(robot[0].x == berth[robot[0].nearestberth_index].x && 
            robot[0].y == berth[robot[0].nearestberth_index].y && robot[0].goods == 1)
        {
#ifdef LOG
            logFile << "Line 285 hit." << endl;
#endif
            printf("pull %d\n", 0);
            goods[robot[0].nearestgoods_index].status = 2;
            robot[0].nearestgoods_index = -1;
            robot[0].goods = 0;
#ifdef LOG
            logFile << "Robot 0 reached the berth! " << endl;
            logFile << "pull 0" << endl;
#endif
        }
        // 机器人到取货点了
        else if(robot[0].x == goods[robot[0].nearestgoods_index].x && 
            robot[0].y == goods[robot[0].nearestgoods_index].y && robot[0].goods == 0 && 
            goods[robot[0].nearestgoods_index].remaintime > 0)
        {
            printf("get %d\n", 0);
#ifdef LOG
            logFile << "Robot 0 reached the goods! " << endl;
            logFile << "robot[0]: (" << robot[0].x << ", " << robot[0].y << ")" << endl;
            logFile << "goods: (" << goods[robot[0].nearestgoods_index].x;
            logFile << ", " << goods[robot[0].nearestgoods_index].y << ") ";
            logFile << "remaintime: " << goods[robot[0].nearestgoods_index].remaintime << endl;
            logFile << "get 0" << endl;
#endif
        }
        // 机器人空闲，分配新任务
        else{
#ifdef LOG
            logFile << "Line 314 hit." << endl;
#endif
            if(robot[0].goods == 1)
            {
                // 带了货物，去泊位
                robot[0].nearestberth_index = nearest_berth(robot[0].x, robot[0].y);
                paths[0] = aStarSearch(ch, robot[0].x, robot[0].y,
                    berth[robot[0].nearestberth_index].x, berth[robot[0].nearestberth_index].y);
                // 路径去掉起点
                paths[0].erase(paths[0].begin());
            }
            else if(robot[0].goods == 0)
            {
                // 没带货物，去找货物
                robot[0].nearestgoods_index = selectGoods(0, robot[0].nearestgoods_index + 1);
                paths[0] = aStarSearch(ch, robot[0].x, robot[0].y,
                    goods[robot[0].nearestgoods_index].x, goods[robot[0].nearestgoods_index].y);
                // 来不及去了，或者货物刷墙里了，那这个机器人就休息一帧
                if(paths[0].size() > (goods[robot[0].nearestgoods_index].remaintime) || paths[0].size() == 0)
                {
#ifdef LOG
                    logFile << "paths[0].size(): " << paths[0].size() << endl;
#endif
                    paths[0].clear();
                    robot[0].nearestgoods_index > 100 ? robot[0].nearestgoods_index = 0 : robot[0].nearestgoods_index ++;

                    // robot[0].nearestgoods_index = selectGoods(0, robot[0].nearestgoods_index + 1);
                    // paths[0] = aStarSearch(ch, robot[0].x, robot[0].y,
                    //     goods[robot[0].nearestgoods_index].x, goods[robot[0].nearestgoods_index].y);
                }
                else{
                    paths[0].erase(paths[0].begin());
                }
#ifdef LOG
                logFile << "nearestgoods_index: " << robot[0].nearestgoods_index << endl;
                if(paths[0].size() != 0)
                {
                    logFile << "goods(x, y): " << "(" << goods[robot[0].nearestgoods_index].x << ", " << goods[robot[0].nearestgoods_index].y << ")" << endl;
                    for(const auto& p : paths[0])
                    {
                        logFile << "(" << p.first << ", " << p.second << ") ";
                    }
                    logFile << endl;
                }
                logFile << "Line 372 hit. " << endl;
#endif
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