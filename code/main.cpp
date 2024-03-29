#define LOG  // Comment this line if no need for log file.
#include "main.h"
#ifdef LOG
#include <chrono>
#endif

/* 
TODO: 
1.10000帧之前，动态泊位；10000帧后，固定泊位。
 修改：262行改为 berth[i].selected。
 berth.cpp，新增一对函数，10000之前的和之后的分开，10000之前的不管selected，之后的关注selected，
 轮船10000帧先去虚拟点倒货,然后去!selected泊位，装完后去selected 泊位，最后结算时再走。
2.最近货物的选择：随机选两个直线距离最近的，然后计算实际距离
*/

char ch[n][n]; // 存储地图
//char astarmap[n][n]; // 用于 A* 的地图
int availmap[n][n]; // 记录可达点的地图（包含陆地、机器人、港口位置）
//int robotmap[n][n];  // 存储当前机器人位置的地图, 内容为机器人的编号
int robotmap_next[n][n]; // 存储下一帧机器人位置的地图, 内容为下一帧机器人的个数
bool narrowmap[n][n];     // 标记仅有一格宽的狭路区域
int gds[n][n]; // 存储当前货物位置(内容为货物编号，-1 为无货)
int boat_capacity; // 船只容量
Berth berth[berth_num];
Robot robot[robot_num];
Goods goods[210];

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
    int x_up = robot[robot_index].x - range, y_left = robot[robot_index].y - range;
    if(x_up < 0)  x_up = 0;
    if(y_left < 0)    y_left = 0;
    // 边界右下角
    int x_down = robot[robot_index].x + range, y_right = robot[robot_index].y + range;
    if(x_down >= n) x_down = n - 1;
    if(y_right >= n)  y_right = n - 1;

    for(int i = x_up; i <= x_down; ++i)
    {
        int j = y_left;
        // 有货且可达
        if(gds[i][j] >= 0 && availmap[i][j] == availmap[robot[robot_index].x][robot[robot_index].y]
        && goods[gds[i][j]].robotindex < 0 && goods[gds[i][j]].val > 100)
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
        j = y_right;
        if(gds[i][j] >= 0 && availmap[i][j] == availmap[robot[robot_index].x][robot[robot_index].y]
        && goods[gds[i][j]].robotindex < 0 && goods[gds[i][j]].val > 100)
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

    for(int j = y_left; j <= y_right; j++)
    {
        int i = x_up;
        // 有货且可达
        if(gds[i][j] >= 0 && availmap[i][j] == availmap[robot[robot_index].x][robot[robot_index].y]
        && goods[gds[i][j]].robotindex < 0 && goods[gds[i][j]].val > 100)
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
        i = x_down;
        // 有货且可达
        if(gds[i][j] >= 0 && availmap[i][j] == availmap[robot[robot_index].x][robot[robot_index].y]
        && goods[gds[i][j]].robotindex < 0 && goods[gds[i][j]].val > 100)
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
    if(x_up == 0 && y_left == 0 && x_down == n - 1 && y_right == n - 1) {
        paths[robot_index].clear();
        return -1;
    }
    // 扩大范围继续找
    return selectnearestGoods(robot_index, range + 1);
}
int selectbestGoods(int robot_index){
    int best_goods_index = -1;
    int goods_index[3] = {-1};
    int range = 1;
    std::vector<std::pair<int, int>> temp_paths = std::vector<std::pair<int, int>>();
    for(int i = 0; i < 3; i ++){
        goods_index[i] = selectnearestGoods(robot_index, range);
        if(goods_index[i] == -1){
            break;
        }
        else{
            goods[goods_index[i]].robotindex = -1;
            range = max(abs(robot[robot_index].x - goods[goods_index[i]].x), abs(robot[robot_index].y - goods[goods_index[i]].y));
            range ++;
            if(temp_paths.empty()){
                temp_paths = paths[robot_index];
                best_goods_index = goods_index[i];
            }
            else{
                temp_paths = (temp_paths.size() < paths[robot_index].size()) ? temp_paths : paths[robot_index];
                best_goods_index = (temp_paths.size() < paths[robot_index].size()) ? best_goods_index : goods_index[i];
            }
        }
    }
    if(best_goods_index >= 0)
        goods[best_goods_index].robotindex = robot_index;
    paths[robot_index] = temp_paths;
    return best_goods_index;
}

// 检查指定位置是否为边界或不可通行
bool isBoundaryOrBlocked(int row, int col){
    if(row < 0 || row >= n || col < 0 || col >n || availmap[row][col] < 0 || ch[row][col] == 'A'){
        return true;
    }
    return false;
}
// 标记狭路区域
void markNarrowArea(){
    // 区域编号
//    int narrowAreaID = 1;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            narrowmap[i][j] = false;
            if(availmap[i][j] >= 0){
                int blockedDirection = 0;
                int availDirection = 0;
                blockedDirection += isBoundaryOrBlocked(i - 1, j) ? 1 : 0;
                blockedDirection += isBoundaryOrBlocked(i + 1, j) ? 1 : 0;
                blockedDirection += isBoundaryOrBlocked(i, j - 1) ? 1 : 0;
                blockedDirection += isBoundaryOrBlocked(i, j + 1) ? 1 : 0;

                availDirection += isBoundaryOrBlocked(i + 1, j + 1) ? 0 : 1;
                availDirection += isBoundaryOrBlocked(i + 1, j - 1) ? 0 : 1;
                availDirection += isBoundaryOrBlocked(i - 1, j + 1) ? 0 : 1;
                availDirection += isBoundaryOrBlocked(i - 1, j - 1) ? 0 : 1;
                availDirection += isBoundaryOrBlocked(i, j + 1) ? 0 : 1;
                availDirection += isBoundaryOrBlocked(i, j - 1) ? 0 : 1;
                availDirection += isBoundaryOrBlocked(i + 1, j) ? 0 : 1;
                availDirection += isBoundaryOrBlocked(i - 1, j) ? 0 : 1;

                if(blockedDirection >= 2 && availDirection <= 3){
                    // 狭路区域遇到则避让，开阔区域交错则等待
                    // 判断：在狭路区域对撞则退格避让，开阔区域对撞则重新A*
                    narrowmap[i][j] = true;
                }

                // 对两面包夹芝士的区域标记
                if(((isBoundaryOrBlocked(i - 1, j) && isBoundaryOrBlocked(i + 1, j))) ||
                        (isBoundaryOrBlocked(i, j - 1) && isBoundaryOrBlocked(i, j + 1)))
                    narrowmap[i][j] = true;
            }
        }
    }
    return;
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
                << robot[robot_number].y << ")" << endl;
#endif                    
                robot_number++;
            }
        }
    }
    // 标记可达点
    initMap(ch, availmap);


#ifdef LOG
    logFile << "availmap: " << endl;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            logFile << setw(2) << setfill(' ') << availmap[i][j];
        }
        logFile << endl;
    }
#endif
    // 标记狭窄区域
    markNarrowArea();

#ifdef LOG

    logFile << "Narrow Map:" << endl;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
//            logFile << setw(2) << setfill(' ') << narrowmap[i][j];
            logFile << narrowmap[i][j];
        }
        logFile << endl;
    }
#endif

    // 检查机器人是否在墙里
    for(int i = 0; i < robot_num; ++i)
    {
        int x = robot[i].x;
        int y = robot[i].y;
        if(availmap[x][y] >= 0)  robot[i].enable = true;
    }

#ifdef LOG
    for(int i = 0; i < robot_num; ++i)
    {
        logFile << "Robot [" << i << "].enable: " << robot[i].enable << endl;
    }
#endif
    
    // 读入泊位信息
    for(int i = 0; i < berth_num; i ++) {
        int id;
        scanf("%d", &id);
        scanf("%d%d%d%d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed);
    }
    //-------------------------BERTH-SELECT--------------------------------//
    // 小块区域的中心点
    int areapoint[5][2] = {
            {60,  50},
            {60,  100},
            {60,  150},
            {130, 67},
            {130, 134}
    };
    for (int area = 0; area < 5; area++) {
        int x = areapoint[area][0], y = areapoint[area][1];
        while (availmap[x][y] < 0) {
            x++;
            y++;
        }
        vector<vector<pair<int, int>>> berth_distance(berth_num);
        int min = 400;
        int selected_berth = -1;
        for (int i = 0; i < berth_num; ++i) {
            if (!berth[i].selected && availmap[berth[i].x][berth[i].y] == availmap[x][y]) {
                berth_distance[i] = aStarSearch(ch, x, y, berth[i].x, berth[i].y);
                if (berth_distance[i].size() > 0 && berth_distance[i].size() < min) {
                    min = berth_distance[i].size();
                    selected_berth = i;
                }
            }
        }
        berth[selected_berth].selected = true;
//        berth[selected_berth].boat_index = area;
#ifdef LOG
        logFile << "Area " << area << " Selected berth: " << selected_berth << ", distance: " << min << endl;
#endif
    }

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
//            astarmap[i][j] = ch[i][j];
            robotmap_next[i][j] = 0;
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
        ch[x][y] = 'A';
        // 把狭路中的机器人去除，从而不影响 A*
//        if(!narrowmap[x][y])
//            astarmap[x][y] = 'A';
        robot[i].actioned = false;

        if(robot[i].goods == 1)
        {
            // 将货物标记为已被拾取
            goods[robot[i].nearestgoods_index].status = 1;
            // 将货物从地图上删去
            gds[goods[robot[i].nearestgoods_index].x][goods[robot[i].nearestgoods_index].y] = -1;
        }
    }
    markNarrowArea();

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
        // 跳帧的时间差
        int time_inaccuracy = id - frame;
        // 修正时间
        frame = id;
#ifdef LOG
        logFile << "====================================================================" << endl;
        logFile << "Frame " << frame << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
#endif
//---------------------------------------BERTH---------------------------------------//
//        for(int berthcnt = 0; berthcnt < berth_num; berthcnt++){
//#ifdef LOG
//            logFile << "berth[" << berthcnt << "]: boat_index: " << berth[berthcnt].boat_index << endl
//            << "boat status: " << boat[berth[berthcnt].boat_index].status << endl
//            << "boat pos: " << boat[berth[berthcnt].boat_index].pos << endl;
//#endif
//            if(berth[berthcnt].boat_index >= 0 && boat[berth[berthcnt].boat_index].status == BOAT_STATUS_NORMAL &&
//                    boat[berth[berthcnt].boat_index].pos == -1){
//                printf("ship %d %d\n", berth[berthcnt].boat_index, berthcnt);
//#ifdef LOG
//                logFile << "ship " << berth[berthcnt].boat_index << " " << berthcnt << std::endl;
//#endif
//            }
//        }
//-----------------------------------ROBOT------------------------------------------------//
        // 本轮执行动作的机器人数量，如果为0就可以退出循环了
        for(int robotcnt = 0; robotcnt < 10; robotcnt++){
            if(!paths[robotcnt].empty()){
                pair<int, int> next_step = paths[robotcnt].front();

                robotmap_next[next_step.first][next_step.second]++;
            }
        }

        while(true) {
            int actioned_bot = 0;
            for (int robotcnt = 0; robotcnt < 10; robotcnt++) {
                if (robot[robotcnt].enable && !robot[robotcnt].actioned && robot[robotcnt].status == 1) {
#ifdef LOG
                    logFile << "Check robot " << robotcnt << endl;
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    logFile << "Time: " << duration.count() << " ms" << endl;
#endif
                    // 机器人闲着
                    if (paths[robotcnt].empty()) {
#ifdef LOG
                        logFile << "nearestberth_index: " << robot[robotcnt].nearestberth_index << endl;
                        logFile << "nearestgoods_index: " << robot[robotcnt].nearestgoods_index << endl;
                        logFile << "robot (x, y): " << robot[robotcnt].x << ", " << robot[robotcnt].y << endl;
#endif
                        // 机器人分配泊位了
                        if(robot[robotcnt].nearestberth_index >= 0)
                        {
#ifdef LOG
                            logFile << "nearestberth(x, y): " << berth[robot[robotcnt].nearestberth_index].x
                            << ", " << berth[robot[robotcnt].nearestberth_index].y << endl;
#endif
                            if (robot[robotcnt].x == berth[robot[robotcnt].nearestberth_index].x &&
                                robot[robotcnt].y == berth[robot[robotcnt].nearestberth_index].y &&
                                robot[robotcnt].goods == 1) {
                                printf("pull %d\n", robotcnt);
                                actioned_bot++;
                                robot[robotcnt].actioned = true;
                                // 将货物标记为已经运输完成
                                goods[robot[robotcnt].nearestgoods_index].status = 2;
                                goods[robot[robotcnt].nearestgoods_index].robotindex = -1;
                                // 港口增加一货物
                                berth[robot[robotcnt].nearestberth_index].goods++;
                                // 卸货的同时清空最近货物和最近泊位的标记
                                robot[robotcnt].nearestgoods_index = -1;
                                robot[robotcnt].nearestberth_index = -1;
                                robot[robotcnt].secondnearestberth = -1;
                                robot[robotcnt].goods = 0;
#ifdef LOG
                                logFile << "Robot " << robotcnt << " reached the berth! " << endl;
                                logFile << "pull " << robotcnt << endl;
#endif
                            }
                            // 带了货物但没去港口
                            else if(robot[robotcnt].goods == 1)
                            {
                                // 比较两个最近港口的实际路线长度
                                if(robot[robotcnt].secondnearestberth >= 0) {
                                    vector<pair<int, int>> temp_path = aStarSearch(ch, robot[robotcnt].x,
                                                                                   robot[robotcnt].y,
                                                                                   berth[robot[robotcnt].secondnearestberth].x,
                                                                                   berth[robot[robotcnt].secondnearestberth].y);
                                    paths[robotcnt] = aStarSearch(ch, robot[robotcnt].x, robot[robotcnt].y,
                                                                  berth[robot[robotcnt].nearestberth_index].x,
                                                                  berth[robot[robotcnt].nearestberth_index].y);
                                    if (!temp_path.empty() && temp_path.size() < paths[robotcnt].size()) {
                                        paths[robotcnt] = temp_path;
                                        robot[robotcnt].nearestberth_index = robot[robotcnt].secondnearestberth;
                                    }
                                }
                                else {
                                    paths[robotcnt] = aStarSearch(ch, robot[robotcnt].x, robot[robotcnt].y,
                                                                  berth[robot[robotcnt].nearestberth_index].x,
                                                                  berth[robot[robotcnt].nearestberth_index].y);
                                }
                                // 路径去掉起点
                                if(!paths[robotcnt].empty()) {
                                    paths[robotcnt].erase(paths[robotcnt].begin());
                                    robotmap_next[paths[robotcnt].front().first][paths[robotcnt].front().second]++;
                                }
#ifdef LOG
                                logFile << "nearest berth: " << robot[robotcnt].nearestberth_index << endl;
#endif
                            }
                            else{
                                robot[robotcnt].nearestberth_index = -1;
                                robot[robotcnt].secondnearestberth = -1;
                            }
                        }
                        // 机器人没取货但已经分配货物
                        else if(robot[robotcnt].nearestgoods_index >= 0)
                        {
                            // 拾取货物
                            if (robot[robotcnt].x == goods[robot[robotcnt].nearestgoods_index].x &&
                                robot[robotcnt].y == goods[robot[robotcnt].nearestgoods_index].y &&
                                robot[robotcnt].goods == 0 &&
                                goods[robot[robotcnt].nearestgoods_index].remaintime > 0 &&
                                goods[robot[robotcnt].nearestgoods_index].status == 0) {

                                printf("get %d\n", robotcnt);
                                robot[robotcnt].goods = 1;
                                actioned_bot++;
                                robot[robotcnt].actioned = true;
                                // 取两个最近的泊位
                                if(frame <= 11000) {
                                    robot[robotcnt].nearestberth_index = nearest_berth(availmap, robot[robotcnt].x,
                                                                                       robot[robotcnt].y);
                                    robot[robotcnt].secondnearestberth = second_nearest_berth(availmap,
                                                                                              robot[robotcnt].x,
                                                                                              robot[robotcnt].y);
                                }
                                else{
                                    robot[robotcnt].nearestberth_index = nearest_berth_after_11000(availmap, robot[robotcnt].x,
                                                                                       robot[robotcnt].y);
                                    robot[robotcnt].secondnearestberth = second_nearest_berth_after_11000(availmap,
                                                                                              robot[robotcnt].x,
                                                                                              robot[robotcnt].y);
                                }
#ifdef LOG
                                logFile << "Robot " << robotcnt << " reached the goods! " << endl;
                                logFile << "robot[" << robotcnt << "]: (" << robot[robotcnt].x << ", " << robot[robotcnt].y
                                        << ")" << endl;
                                logFile << "goods: (" << goods[robot[robotcnt].nearestgoods_index].x;
                                logFile << ", " << goods[robot[robotcnt].nearestgoods_index].y << ") ";
                                logFile << "remaintime: " << goods[robot[robotcnt].nearestgoods_index].remaintime << endl;
                                logFile << "get " << robotcnt << endl;
                                logFile << "nearest berth: " << robot[robotcnt].nearestberth_index << endl << "  berth boatindex: "
                                << berth[robot[robotcnt].nearestberth_index].boat_index << endl;
                                logFile << "second nearest berth: " << robot[robotcnt].secondnearestberth << endl << "  berth boatindex: "
                                << berth[robot[robotcnt].secondnearestberth].boat_index << endl;

#endif
                            }
                            // 要去拿货物但是还没到
                            else if(robot[robotcnt].goods == 0 && goods[robot[robotcnt].nearestgoods_index].remaintime > 0 &&
                                    goods[robot[robotcnt].nearestgoods_index].status == 0){
                                /* 执行A* */
                                paths[robotcnt] = aStarSearch(ch, robot[robotcnt].x, robot[robotcnt].y,
                                                              goods[robot[robotcnt].nearestgoods_index].x,
                                                              goods[robot[robotcnt].nearestgoods_index].y);
                                if(!paths[robotcnt].empty()) {
                                    paths[robotcnt].erase(paths[robotcnt].begin());
                                    robotmap_next[paths[robotcnt].front().first][paths[robotcnt].front().second]++;
                                }
                            }
                            else{
                                robot[robotcnt].nearestgoods_index = -1;
                            }
                        }
                        // 机器人空闲，分配新货物
                        else {
                            if(frame < 500)
                                robot[robotcnt].nearestgoods_index = selectnearestGoods(robotcnt, 1);
                            else
                                robot[robotcnt].nearestgoods_index = selectbestGoods(robotcnt);
#ifdef LOG
                            end = std::chrono::high_resolution_clock::now();
                            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                            logFile << "Time: " << duration.count() << " ms" << endl;

                            logFile << "nearestgoods_index: " << robot[robotcnt].nearestgoods_index << endl;
                            if (paths[robotcnt].size() != 0) {
                                logFile << "goods(x, y): " << "(" << goods[robot[robotcnt].nearestgoods_index].x
                                        << ", " << goods[robot[robotcnt].nearestgoods_index].y << ")" << endl;
                            }
#endif
                            // 没找到货物，一边去，别挡路
                            if(robot[robotcnt].nearestgoods_index == -1){
                                for(int berthcnt = 0; berthcnt < berth_num; berthcnt++){
                                    if(robot[robotcnt].x == berth[berthcnt].x && robot[robotcnt].y == berth[berthcnt].y){
                                        paths[robotcnt] = aStarSearch(ch, robot[robotcnt].x, robot[robotcnt].y, robot[robotcnt].x + 3,
                                                                      robot[robotcnt].y + 3);
                                        if(!paths[robotcnt].empty()) {
                                            paths[robotcnt].erase(paths[robotcnt].begin());
                                            robotmap_next[paths[robotcnt].front().first][paths[robotcnt].front().second]++;
                                        }
                                        break;
                                    }
//                                    if(robot[robotcnt].x == berth[berthcnt].x + 3 && robot[robotcnt].y == berth[berthcnt].y + 3) {
//                                        robot[robotcnt].actioned = true;
//                                        break;
//                                    }
                                }
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
                            // 移动失败，发生碰撞
                        {
                            robotmap_next[next_step.first][next_step.second]--;
#ifdef LOG
                            logFile << "Robot " << robotcnt << " moved failed." << endl;
#endif
                            if(robotmap_next[robot[robotcnt].x][robot[robotcnt].y] > 0) {
#ifdef LOG
                                logFile << "Avoidance strategy activated." << endl;
#endif
                                // 如果对方机器人在狭路区域，执行避让
                                if (narrowmap[next_step.first][next_step.second]) {
#ifdef LOG
                                    logFile << "Avoidance action." << endl;
#endif
                                    int x = robot[robotcnt].x, y = robot[robotcnt].y;
                                    // 计算动量
                                    int mhx = next_step.first - x, mhy = next_step.second - y;
                                    pair<int, int> newPoint = {x, y};
                                    paths[robotcnt].insert(paths[robotcnt].begin(), newPoint);
                                    // 还要计算避让点的坐标
                                    // 优先计算横向避让
                                    srand(static_cast<unsigned int>(time(0)));
                                    int avoidance_coefficient = rand() % 2;
                                    if(avoidance_coefficient == 0) {
                                        if (ch[x + mhy][y + mhx] == '.' || ch[x + mhy][y + mhx] == 'B') {
                                            newPoint = {x + mhy, y + mhx};
                                            paths[robotcnt].insert(paths[robotcnt].begin(), newPoint);
                                        } else if (ch[x - mhy][y - mhx] == '.' || ch[x - mhy][y - mhx] == 'B') {
                                            newPoint = {x - mhy, y - mhx};
                                            paths[robotcnt].insert(paths[robotcnt].begin(), newPoint);
                                        }
                                            // 狭路相逢，纵向避让
                                        else if (ch[x - mhx][y - mhy] == '.' || ch[x - mhx][y - mhy] == 'B') {
                                            newPoint = {x - mhx, y - mhy};
                                            paths[robotcnt].insert(paths[robotcnt].begin(), newPoint);
                                        }
                                            // 卡死了，动不了
                                        else {
                                            robot[robotcnt].actioned = true;
                                        }
                                    }
                                    else{
                                        if (ch[x - mhy][y - mhx] == '.' || ch[x - mhy][y - mhx] == 'B') {
                                            newPoint = {x - mhy, y - mhx};
                                            paths[robotcnt].insert(paths[robotcnt].begin(), newPoint);
                                        } else if (ch[x + mhy][y + mhx] == '.' || ch[x + mhy][y + mhx] == 'B') {
                                            newPoint = {x + mhy, y + mhx};
                                            paths[robotcnt].insert(paths[robotcnt].begin(), newPoint);
                                        }
                                            // 狭路相逢，纵向避让
                                        else if (ch[x - mhx][y - mhy] == '.' || ch[x - mhx][y - mhy] == 'B') {
                                            newPoint = {x - mhx, y - mhy};
                                            paths[robotcnt].insert(paths[robotcnt].begin(), newPoint);
                                        }
                                            // 卡死了，动不了
                                        else {
                                            robot[robotcnt].actioned = true;
                                        }
                                    }
                                }
                                // 对面不在狭窄区域，但这个机器人在
                                else if(narrowmap[robot[robotcnt].x][robot[robotcnt].y]){
#ifdef LOG
                                    logFile << "Waiting for another robot to avoid crash..." << endl;
#endif
                                    robotmap_next[next_step.first][next_step.second]++;
                                    continue;
                                }
                                    // 重新计算路线
                                else {
                                    paths[robotcnt].clear();
                                }
                                if (!paths[robotcnt].empty()) {
                                    robotmap_next[paths[robotcnt].front().first][paths[robotcnt].front().second]++;
                                }
                                // 对本个机器人再次执行移动计算。
                                robotcnt--;
                                continue;
                            }
                            else{
                                // 原地等待
                                robot[robotcnt].actioned = true;
                            }
                        }
                        else if (move_result == 1) {
                            // 移动成功，并且没到目的地，机器人操作结束
                            // 如果到目的地，执行取、放货操作
                            paths[robotcnt].erase(paths[robotcnt].begin());
                            if (!paths[robotcnt].empty() || (robot[robotcnt].nearestgoods_index < 0 && robot[robotcnt].nearestberth_index < 0)) {
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
            // 对虚拟点的船找一个货最多的泊位
            if(frame < 8500 && boat[i].status == BOAT_STATUS_NORMAL){
                if(boat[i].pos == -1) {
                    int berthid = -1;
                    int max_berth_goods = -1;
                    for (int j = 0; j < berth_num; j++) {
                        if (berth[j].goods > max_berth_goods && berth[j].boat_index == -1) {
                            berthid = j;
                            max_berth_goods = berth[j].goods;
                        }
                    }
                    printf("ship %d %d\n", i, berthid);
                    boat[i].goods = 0;
                    berth[berthid].boat_index = i;
                    boat[i].pos = berthid;
                }
                else if(boat[i].pos >= 0 && boat[i].goods < boat_capacity - 12){
                    // 货装完了，去别的泊位
                    if(berth[boat[i].pos].goods <= 2){
                        berth[boat[i].pos].boat_index = -1;
                        int berthid = -1;
                        int max_berth_goods = -1;
                        for (int j = 0; j < berth_num; j++) {
                            if (berth[j].goods > max_berth_goods && berth[j].boat_index == -1) {
                                berthid = j;
                                max_berth_goods = berth[j].goods;
                            }
                        }
                        printf("ship %d %d\n", i, berthid);
                        berth[berthid].boat_index = i;
                        boat[i].pos = berthid;
                    }
                }
            }
            if(frame == 10000){
                printf("go %d\n", i);
                if(boat[i].pos >= 0) {
                    berth[boat[i].pos].boat_index = -1;
                    boat[i].pos = -1;
                }
            }
            if(frame > 10500 && frame < 13000){
                if(boat[i].pos == -1 && boat[i].status == BOAT_STATUS_NORMAL) {
                    for (int j = 0; j < berth_num; j++) {
                        if (!berth[j].selected && berth[j].boat_index == -1) {
                            printf("ship %d %d\n", i, j);
                            boat[i].goods = 0;
                            boat[i].pos = j;
                            berth[j].boat_index = i;
                            break;
                        }
                    }
                }
            }
            if(frame > 12000){
                if(boat[i].pos >= 0 && boat[i].status == BOAT_STATUS_NORMAL){
                    if(!berth[boat[i].pos].selected && berth[boat[i].pos].goods <= 0){
                        for(int j = 0; j < berth_num; j++){
                            if(berth[j].selected && berth[j].boat_index == -1){
                                printf("ship %d %d\n", i, j);
                                berth[boat[i].pos].boat_index = -1;
                                boat[i].pos = j;
                                berth[j].boat_index = i;
                                break;
                            }
                        }
                    }
                }
            }
            // 如果船装满了货，就开走
            if(boat[i].goods >= boat_capacity && boat[i].status == BOAT_STATUS_NORMAL && boat[i].pos >= 0)
            {
                berth[boat[i].pos].boat_index = -1;
                printf("go %d\n", i);
                // 清空船上的货物计数器
                boat[i].goods = 0;
            }
            // 即将结算时开走所有船
            if(boat[i].status == BOAT_STATUS_NORMAL && boat[i].pos >= 0){
                if((berth[boat[i].pos].transport_time + frame + 1) >= 15000) {
                    printf("go %d\n", i);
                    boat[i].goods = 0;
                    berth[boat[i].pos].boat_index = -1;
                }
            }
        }

//----------------------------------GOODS-----------------------------------//
        // 更新每一个货物的存活时间
        for(int t = 0; t <= time_inaccuracy; ++t) {
            for (int i = 0; i < 210; i++) {
                if (goods[i].remaintime > 0 && goods[i].status == 0) {
                    goods[i].remaintime--;
                    if (goods[i].remaintime <= 0) {
                        goods[i].status = 2;
                        gds[goods[i].x][goods[i].y] = -1;
                        // 这里把对应机器人的货物索引清空
                        if(goods[i].robotindex >= 0){
                            robot[goods[i].robotindex].nearestgoods_index = -1;
                        }
                        goods[i].robotindex = -1;
                    }
                }
            }
        }

//----------------------------------BERTH----------------------------------//
        // 检查港口
        for(int t = 0; t <= time_inaccuracy; ++t) {
            for (int i = 0; i < berth_num; ++i) {
                // 港口将货物装载到船上
                if (berth[i].goods > 0 && berth[i].boat_index != -1 &&
                    boat[berth[i].boat_index].status == BOAT_STATUS_NORMAL) {
                    if(boat[berth[i].boat_index].pos == i) {
                        // 如果船没装满
                        if (boat[berth[i].boat_index].goods < boat_capacity) {
                            // 如果港口堆积的货物比 loading_speed 要多，并且船剩下的容量也大于 loading_speed
                            if ((berth[i].goods > berth[i].loading_speed) &&
                                ((boat_capacity - boat[berth[i].boat_index].goods) > berth[i].loading_speed)) {
                                // 那么就装 loading_speed 个货
                                boat[berth[i].boat_index].goods += berth[i].loading_speed;
#ifdef LOG
                                logFile << "boat[" << berth[i].boat_index << "].goods: "
                                        << boat[berth[i].boat_index].goods << endl;
#endif
                                berth[i].goods -= berth[i].loading_speed;
                            } else {
                                // 否则把港口的货全装上，或者把船装满，两者满足其一
                                boat[berth[i].boat_index].goods += min(berth[i].goods,
                                                                       boat_capacity - boat[berth[i].boat_index].goods);
                                berth[i].goods -= min(berth[i].goods, boat_capacity - boat[berth[i].boat_index].goods);
                            }
                        }
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