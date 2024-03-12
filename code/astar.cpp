#include "astar.h"

using namespace std;

// 存储节点的位置、父节点和代价
struct Node 
{
    int parentX, parentY;
    int x, y;
    float f, g, h;
};

// 优先队列的比较函数
struct comp
{
    bool operator()(const Node& lhs, const Node& rhs) const
    {
        return lhs.f > rhs.f;
    }
};

/*
    @brief: 判断给定节点是否为目标节点
    @param: x, y: 节点位置; destX, destY: 目标节点坐标
    @ret: true 代表为目标节点
*/
bool isDestination(int x, int y, int destX, int destY)
{
    return (x == destX && y == destY);
}

/*
    @brief: 计算曼哈顿距离
    @param: x, y: 节点位置; destX, destY: 目标节点位置
    @ret: 曼哈顿距离
*/
float calculateH(int x, int y, int destX, int destY)
{
    return static_cast<float>(abs(x - destX) + abs(y - destY));
}

/*
    @brief: 判断节点是否可达
    @param: x, y: 节点位置
    @ret: true 为可走
*/
bool isUnBlocked(char grid[n][n], int x, int y)
{
    if(x >= 0 && x < n && y >= 0 && y < n && (grid[x][y] == '.' || grid[x][y] == 'A' || grid[x][y] == 'B'))
    {
        return true;
    }
    return false;
}

/*
    @brief: A* 算法
    @param: srcX, srcY: 起始点坐标; destX, destY: 终点坐标
    @ret: none
*/
std::vector<std::pair<int, int>> aStarSearch(char grid[n][n], int srcX, int srcY, int destX, int destY)
{
    // 若起点或终点不可达
    if(!isUnBlocked(grid, srcX, srcY) || !isUnBlocked(grid, destX, destY))
    {
        return std::vector<std::pair<int, int>>();
        // return {{isUnBlocked(grid, srcX, srcY), isUnBlocked(grid, destX, destY)}, {1, 2}};
    }

    bool closedList[n][n];

    memset(closedList, false, sizeof(closedList));
    Node nodes[n][n];

    int dx[] = {-1,  1,  0, 0};
    int dy[] = { 0,  0, -1, 1}; // 上下左右

    // Initialize nodes.
    for(int x = 0; x < n; ++x)
    {
        for(int y = 0; y < n; ++y)
        {
            nodes[x][y].f = FLT_MAX;
            nodes[x][y].g = FLT_MAX;
            nodes[x][y].h = FLT_MAX;
            nodes[x][y].x = x;
            nodes[x][y].y = y;
            nodes[x][y].parentX = -1;
            nodes[x][y].parentY = -1;
        }
    }

    // Initialize source position.
    int i = srcX, j = srcY;
    nodes[i][j].f = 0.0;
    nodes[i][j].g = 0.0;
    nodes[i][j].h = 0.0;
    nodes[i][j].parentX = i;
    nodes[i][j].parentY = j;

    priority_queue<Node, vector<Node>, comp> openList; // 按 f 值排序
    openList.push(nodes[i][j]);

    while(!openList.empty())
    {

        Node node = openList.top();
        openList.pop();
        i = node.x;
        j = node.y;
        closedList[i][j] = true;

        for(int k = 0; k < 4; ++k)
        {
            int newX = i + dx[k];
            int newY = j + dy[k];


            // 找到目的地
            if(isDestination(newX, newY, destX, destY))
            {
                nodes[newX][newY].parentX = i;
                nodes[newX][newY].parentY = j;

                // 给出路径
                vector<pair<int, int>> path;
                while(!(nodes[newX][newY].parentX == newX && nodes[newX][newY].parentY == newY))
                {
                    path.push_back(make_pair(newX, newY));
                    int tempX = nodes[newX][newY].parentX;
                    newY = nodes[newX][newY].parentY;
                    newX = tempX;
                }
                path.push_back(make_pair(srcX, srcY));
                reverse(path.begin(), path.end());

                return path;
            }
            
            // Check if the node can be calculated.
            if(isUnBlocked(grid, newX, newY) && !closedList[newX][newY])
            {
                float gNew = nodes[i][j].g + 1.0;
                float hNew = calculateH(newX, newY, destX, destY);
                float fNew = gNew + hNew;


                if(nodes[newX][newY].f > fNew)
                {
                    if(nodes[newX][newY].f == FLT_MAX)
                    {
                        openList.push(nodes[newX][newY]);
                    }
                    nodes[newX][newY].f = fNew;
                    nodes[newX][newY].g = gNew;
                    nodes[newX][newY].h = hNew;
                    nodes[newX][newY].parentX = i;
                    nodes[newX][newY].parentY = j;
                }
            }
        }
    }
    // Return an empty path when failed.
    return vector<pair<int, int>>();
    // return {{2, 7}};
}