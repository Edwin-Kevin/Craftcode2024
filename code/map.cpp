#include "map.h"

using namespace std;

const int dx[4] = {-1, 1, 0, 0};
const int dy[4] = {0, 0, -1, 1};

/* 
    @brief: 在初始化中查找地图可达点
    @params: ch: 给入的地图; outputMap: 输出的可达性地图(不可达为 false)
    @ret: std::vector<int>
*/
void initMap(char ch[n][n], int outputMap[n][n])
{
    // Initialize outputMap. 
    for(int i = 0; i < n; ++i)
    {
        for(int j = 0; j < n; ++j)
        {
            outputMap[i][j] = -1;
        }
    }

    queue<pair<int, int>> q;
    int areaID = 0;
    for(int i = 0; i < n; ++i)
    {
        for(int j = 0; j < n; ++j)
        {
            if(ch[i][j] == 'B' && outputMap[i][j] < 0)
            {
                q.push({i, j});
                outputMap[i][j] = areaID;
                while(!q.empty())
                {
                    auto [x, y] = q.front();
                    q.pop();

                    for(int i = 0; i < 4; ++i)
                    {
                        int nx = x + dx[i];
                        int ny = y + dy[i];

                        if(nx >= 0 && nx < n && ny >= 0 && ny < n && outputMap[nx][ny] < 0 &&
                                (ch[nx][ny] == 'B' || ch[nx][ny] == 'A' || ch[nx][ny] == '.'))
                        {
                            q.push({nx, ny});
                            outputMap[nx][ny] = areaID;
                        }
                    }
                }
                areaID++;
            }
        }
    }
}
