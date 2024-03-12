#define LOG  // Comment this line if no need for log file.
#include "main.h"

char ch[n][n]; // 存储地图
int gds[n][n]; // 存储当前货物位置
int boat_capacity;
Berth berth[berth_num];
Robot robot[robot_num];
// set 可以自动排序元素，使元素保持升序
set<int> minWeights;
// 使用 multimap 来存储权重值和对应的 berth 索引
multimap<int, int> weightToIndex;

#ifdef LOG
std::ofstream logFile("log.txt");
#endif

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

        // 测试 A*
        if(frame == 1)
        {
            robot[0].nearestberth_index = nearest_berth(robot[0].x, robot[0].y);
#ifdef LOG
            logFile << "nearestberthIdx: " << robot[0].nearestberth_index << endl;
#endif

            auto path = aStarSearch(ch, robot[0].x, robot[0].y,
             berth[robot[0].nearestberth_index].x, berth[robot[0].nearestberth_index].y);

            

#ifdef LOG
            logFile << "Astar src(x, y): " << robot[0].x << ", " << robot[0].y << endl;
            logFile << "Astar dst(x, y): " << berth[robot[0].nearestberth_index].x << ", " 
            << berth[robot[0].nearestberth_index].y << endl;
        
            logFile << "path: " << endl;
            for(const auto& elem : path)
            {
                logFile << "(" << elem.first << ", " << elem.second << ")" << endl;
            }
#endif            
        }

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