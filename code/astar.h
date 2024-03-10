#include <iostream>
#include <vector>
#include <queue>
#include <cmath>

using namespace std;


struct Point 
{
    int x, y;
    // 成本估计，f = g + h
    int f, g, h;
    // 父节点
    Point* parent;

    Point(int _x, int _y) : x(_x), y(_y), f(0), g(0), h(0), parent(nullptr) {}

    // 计算当前节点到目标节点的距离
    int calculateH(const Point& end) const {
        //曼哈顿距离，不考虑对角线，会更快一点
        return abs(end.x - x) + abs(end.y - y);
        //欧几里得距离，稍微慢一点
        //return sqrt(pow(end.x - x, 2) + pow(end.y - y, 2));
    }

    // 计算总代价
    int calculateF() const {
        return g + h;
    }
};

// 定义比较函数，用于优先队列中节点的排序
struct Compare {
    bool operator()(const Point* a, const Point* b) const {
        return a->f > b->f;
    }
};

/* 
    @brief: 查找所有相邻块
    @params: grid: 整张地图; curr: 当前点; end: 终点
    @ret: 所有相邻块
*/
vector<Point> findNeighbors(const vector<vector<char>>& grid, const Point& curr, const Point& end) {
    // 上下左右四个动作的坐标变化
    static const vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    vector<Point> neighbors;

    for (const auto& dir : directions) {
        int newX = curr.x + dir.first;
        int newY = curr.y + dir.second;
        if (newX >= 0 && newX < grid.size() && newY >= 0 && newY < grid[0].size() && 
        (grid[newX][newY] == '.' || grid[newX][newY] == 'A')) {
            Point neighbor(newX, newY);
            neighbor.g = curr.g + 1;
            neighbor.h = neighbor.calculateH(end);
            neighbor.f = neighbor.calculateF();
            neighbors.push_back(neighbor);
        }
    }
    return neighbors;
}

/* 
    @brief: A* 算法
    @params: grid: 地图; start: 起点; end: 终点
    @ret: 计算得到的路径
*/
vector<Point> AStar(const vector<vector<char>>& grid, const Point& start, const Point& end) {
    // 优先队列类型的 openSet ，根据 Compare 函数排序
    priority_queue<Point*, vector<Point*>, Compare> openSet;
    vector<vector<bool>> closedSet(grid.size(), vector<bool>(grid[0].size(), false));

    openSet.push(new Point(start));

    while (!openSet.empty()) {
        Point* curr = openSet.top();
        openSet.pop();

        if (curr->x == end.x && curr->y == end.y) {
            // 找到终点，回溯路径
            vector<Point> path;
            while (curr != nullptr) {
                path.push_back(*curr);
                curr = curr->parent;
            }
            return path;
        }

        // 将已经计算的节点加入关闭集合
        closedSet[curr->x][curr->y] = true;

        // 查找相邻节点
        vector<Point> neighbors = findNeighbors(grid, *curr, end);
        for (auto& neighbor : neighbors) {
            if (!closedSet[neighbor.x][neighbor.y]) {
                neighbor.parent = curr;
                openSet.push(new Point(neighbor));
            }
        }
    }
    return {}; // 没有找到路径
}