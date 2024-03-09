#include "main.h"

int money, id;
char ch[N][N];
int gds[N][N];
// set 可以自动排序元素，使元素保持升序
set<int> minWeights;
// 使用 multimap 来存储权重值和对应的 berth 索引
multimap<int, int> weightToIndex;

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
        // 计算该泊位的权重
        berth[i].weight = -200 * berth[i].loading_speed + 1 * berth[i].transport_time;
        // 将权重存入集合
        weightToIndex.insert(make_pair(berth[i].weight, i)); // 创建了一个包含权重和索引的键值对
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
        int id = Input();
        // 输出对机器人和船的操作指令
        for(int i = 0; i < robot_num; ++ i)
        {
            printf("move %d %d\n", i, rand() % 4);
        }

        // 输出最小的五个 weight 以及它们对应的 berth 索引，从而选择五个最好的港口
        int cnt = 0;
        for (int weight : minWeights) {
            auto range = weightToIndex.equal_range(weight); // 在 weightToIndex 中查找 weight 值
            // range.first 和 range.second 分别是迭代器，指向匹配的第一个元素和超出匹配元素序列的元素
            for (auto it = range.first; it != range.second; ++it) {
                int berthIndex = it -> second;
                // 船只在虚拟点且可移动
                if(boat[cnt].status == BOAT_STATUS_NORMAL && boat[cnt].pos == -1)
                {
                    printf("ship %d %d\n", cnt, berthIndex);
                }
                ++cnt;
            }
        }

        // 移动船只
        for(int i = 0; i < boat_num; ++ i)
        {
            // // 如果船在虚拟点
            // if(boat[i].status == BOAT_STATUS_NORMAL && boat[i].pos == -1)
            //     printf("ship %d %d\n", i, i * 2);


            // 如果船装满了货，就开走
            if(boat[i].goods >= boat_capacity && boat[i].status == BOAT_STATUS_NORMAL)
            {
                printf("go %d\n", i);
            }
        }
        puts("OK");
        fflush(stdout);
    }

    return 0;
}
