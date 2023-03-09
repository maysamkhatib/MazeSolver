#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>
#include "API.h"
using namespace std;
#define SIZE 16
const int INF = 1e9 + 5;
vector<vector<int>> distances;
vector<vector<bool>> isVisited;
vector<vector<int>> path;
pair<int, int> previous[SIZE][SIZE];
vector<vector<int>> previousPath;
vector<vector<int>> goals = {{SIZE/2 - 1, SIZE/2 - 1},
                             {SIZE/2 - 1, SIZE/2},
                             {SIZE/2, SIZE/2 - 1},
                             {SIZE/2, SIZE/2}};
set<pair<int, int>> neighbors[SIZE][SIZE];
int directionsX[4] = {1, -1, 0, 0};
int directionsY[4] = {0, 0, 1, -1};

bool inMaze(int x, int y);

bool reachedGoal(int x, int y);

vector<vector<int>> around(int x, int y, int direction);

void exploreMaze();

void floodFill(int x, int y);

bool seeRight(int x, int y, int direction, pair<int, int> pair);

bool seeLeft(int x, int y, int direction, pair<int, int> pair);

bool seeFront(int x, int y, int direction, pair<int, int> pair);

int goToStartPoint(int x, int y, int x0, int y0, int dir, pair<int, int> parent[16][16], bool ok);

int getPath(int x, int y, int xx, int yy, int direction, bool ok);

int main() {
    distances = vector<vector<int>> (SIZE, vector<int> (SIZE, INF));
    isVisited = vector<vector<bool>> (SIZE, vector<bool> (SIZE, false));
    for (auto & previou : previous) {
        for (auto & j : previou) {
            j = {-1, -1};
        }
    }
    vector<vector<int>> queue;
    for (vector<int> &g: goals) {
        distances[g[0]][g[1]] = 0;
        queue.push_back({0, g[0], g[1]});
        API::setText(g[0], g[1], to_string(distances[g[0]][g[1]]));
    }
    while (!queue.empty()){
        vector<int> tmp = queue[0];
        queue.erase(queue.begin());
        int dis = tmp[0];
        int x = tmp[1];
        int y = tmp[2];
        for (int i = 0; i < 4; ++i) {
            int nextX = x + directionsX[i];
            int nextY = y + directionsY[i];
            if (!inMaze(nextX, nextY))
                continue;
            if (dis + 1 >= distances[nextX][nextY])
                continue;
            distances[nextX][nextY] = dis+1;
            API::setText(nextX, nextY, to_string(distances[nextX][nextY]));
            queue.push_back({dis+1, nextX, nextY});
        }
        sort(queue.begin(), queue.end());
    }
    exploreMaze();
    return 0;
}

void exploreMaze() {
    int direction = 0;
    int x = 0;
    int y = 0;
    for (auto &g: goals) {
        isVisited[g[0]][g[1]] = false;
    }
    while (true){
        API::setColor(x, y, 'G');
        isVisited[SIZE-y-1][x] = true;
        path.push_back({x, y});
        if (reachedGoal(x, y))
            break;
        vector<vector<int>> ar = around(x, y, direction);
        /*cerr << "New\n";
        for (int i = 0; i < ar.size(); ++i) {
            for (int j = 0; j < ar[i].size(); ++j) {
                cerr << ar[i][j] << " ";
            }
            cerr << "\n";
        }*/
        if (distances[SIZE-y-1][x] <= ar[0][0]){
            floodFill(SIZE-y-1, x);
            ar = around(x, y, direction);
        }
        cerr << "\nNew\n";
        for (int i = 0; i < ar.size(); ++i) {
            for (int j = 0; j < ar[i].size(); ++j) {
                cerr << ar[i][j] << " ";
            }
            cerr << "\n";
        }
        cerr << "dir: " << direction << "\nar: " << ar[0][1];
        pair<int, int> dir = API::getDirection(direction, ar[0][1]);
        if(ar[0][1] == 1){
            direction = (direction+1)%4;
            API::turnRight();
        }
        else if(ar[0][1] == 2){
            direction = ((direction-1)%4+4)%4;
            API::turnLeft();
        }
        else if(ar[0][1] == 3){
            direction = (direction+2)%4;
            API::turnRight();
            API::turnRight();
        }
        API::moveForward();
        previous[x + dir.first][y + dir.second] = {x, y};
        x += dir.first;
        y += dir.second;
    }
    if(path == previousPath){
        direction = getPath(SIZE-1, 0, SIZE-y-1, x, direction, 0);
        API::clearAllColor();
        while(direction != 0){
            direction = (direction+1)%4;
            API::turnRight();
        }
        vector<int> v;
        for(auto &g : goals){
            if(isVisited[g[0]][g[1]]){
                v = g;
                break;
            }
        }
        direction = getPath(v[0], v[1], SIZE-1, 0, direction, 1);
        return;
    }
    previousPath = path;
    path.clear();
    direction = getPath(SIZE-1, 0, SIZE-y- 1, x, direction, 0);
    while(direction != 0){
        direction = (direction+1)%4;
        API::turnRight();
    }
    exploreMaze();
}

void floodFill(int x, int y) {
    vector<vector<int>> queue;
    queue.push_back({x, y});
    while(!queue.empty()){
        vector<int> v = queue[0];
        queue.erase(queue.begin());
        x = v[0], y = v[1];
        if(isVisited[x][y]){
            int mn = INF;
            for(pair<int, int> n : neighbors[x][y]){
                int nx = n.first, ny = n.second;
                mn = min(mn, distances[nx][ny]);
            }
            if(distances[x][y] <= mn){
                distances[x][y] = mn + 1;
                API::setText(y, SIZE-x-1, to_string(distances[x][y]));
                for(pair<int, int> n : neighbors[x][y]){
                    int nextX = n.first;
                    int nextY = n.second;
                    queue.push_back({nextX, nextY});
                }
            }
        }
        else{
            int mn = INF;
            for(int i = 0; i < 4; i++){
                int nextX = x + directionsX[i];
                int nextY = y + directionsY[i];
                if(!inMaze(nextX, nextY))
                    continue;
                mn = min(mn, distances[nextX][nextY]);
            }
            if(distances[x][y] <= mn){
                distances[x][y] = mn + 1;
                API::setText(y, SIZE-x-1, to_string(distances[x][y]));
                for(int i = 0; i < 4; i++){
                    int nextX = x + directionsX[i];
                    int nextY = y + directionsY[i];
                    if(!inMaze(nextX, nextY))
                        continue;
                    queue.push_back({nextX, nextY});
                }
            }
        }
    }
}

vector<vector<int>> around(int x, int y, int direction) {
    vector<vector<int>> result;
    pair<int, int> dir;
    dir = API::getDirection(direction, 0);
    if (seeFront(x, y, direction, dir)){
        result.push_back({distances[SIZE-(y+dir.second)-1][x+dir.first], 0});
        neighbors[SIZE-y-1][x].insert({SIZE-(y+dir.second)-1, x+dir.first});
        neighbors[SIZE-(y+dir.second)-1][x+dir.first].insert({SIZE-y-1,x});
    }
    dir = API::getDirection(direction, 1);
    if (seeRight(x, y, direction, dir)){
        result.push_back({distances[SIZE-(y+dir.second)-1][x+dir.first], 1});
        neighbors[SIZE-y-1][x].insert({SIZE-(y+dir.second)-1, x+dir.first});
        neighbors[SIZE-(y+dir.second)-1][x+dir.first].insert({SIZE-y-1, x});
    }
    dir = API::getDirection(direction, 2);
    if(seeLeft(x, y, direction, dir)){
        result.push_back({distances[SIZE-(y+dir.second)-1][x+dir.first], 2});
        neighbors[SIZE-y-1][x].insert({SIZE-(y+dir.second)-1, x+dir.first});
        neighbors[SIZE-(y+dir.second)-1][x + dir.first].insert({SIZE-y-1, x});
    }
    if(previous[x][y].first != -1){
        int prevX = previous[x][y].first, prevY = previous[x][y].second;
        result.push_back({distances[SIZE-prevY-1][prevX], 3});
    }
    sort(result.begin(), result.end());
    return result;
}

bool seeFront(int x, int y, int direction, pair<int, int> pair) {
    if(!API::wallFront() and inMaze(x + pair.first, y + pair.second))
        return true;
    else
        return false;
}

bool seeLeft(int x, int y, int direction, pair<int, int> pair) {
    if(!API::wallLeft() and inMaze(x + pair.first, y + pair.second))
        return true;
    else
        return false;
}

bool seeRight(int x, int y, int direction, pair<int, int> pair) {
    if(!API::wallRight() and inMaze(x + pair.first, y + pair.second))
        return true;
    else
        return false;
}

bool reachedGoal(int x, int y) {
    if ((x == SIZE/2 or x == SIZE/2 - 1) and (y == SIZE/2 or y == SIZE/2 - 1))
        return true;
    else
        return false;
}

bool inMaze(int x, int y) {
    if (x < 0 or x >= SIZE or y < 0 or y >= SIZE)
        return false;
    else
        return true;
}


int goToStartPoint(int x, int y, int x0, int y0, int dir, pair<int, int> (*parent)[SIZE], bool ok = 0){
    string path = "";
    int x00 = x0, y00 = y0;
    while(parent[x0][y0].first != -1){
        pair<int, int> p = parent[x0][y0];
        if(p.first == x0){
            if(y0 < p.second){
                path += 'R';
            }
            else{
                path += 'L';
            }
        }
        else{
            if(x0 < p.first){
                path += 'D';
            }
            else{
                path += 'U';
            }
        }
        x0 = p.first, y0 = p.second;
    }
    int must = 3 * (path[0] == 'L') + 2 * (path[0] == 'D') + 1 * (path[0] == 'R');
    while(dir != must){
        dir = (dir + 1) % 4;
        API::turnRight();
    }
    for(char c : path){
        if(ok){
            API::setColor(y00, SIZE - x00 - 1, 'G');
        }
        pair<int, int> p = parent[x00][y00];
        x00 = p.first, y00 = p.second;
        if(c == 'L'){
            if(dir == 0){
                dir = 3; API::turnLeft();
            }
            else if(dir == 2){
                dir = 3; API::turnRight();
            }
            else if(dir == 3){

            }
            else{
                assert(false);
            }
        }
        else if(c == 'R'){
            if(dir == 0){
                dir = 1; API::turnRight();
            }
            else if(dir == 2){
                dir = 1; API::turnLeft();
            }
            else if(dir == 1){

            }
            else{
                assert(false);
            }
        }
        else if(c == 'U'){
            if(dir == 1){
                dir = 0; API::turnLeft();
            }
            else if(dir == 3){
                dir = 0; API::turnRight();
            }
            else if(dir == 0){

            }
            else{
                assert(false);
            }
        }
        else{
            if(dir == 1){
                dir = 2; API::turnRight();
            }
            else if(dir == 3){
                dir = 2; API::turnLeft();
            }
            else if(dir == 2){

            }
            else{
                assert(false);
            }
        }
        API::moveForward();
    }
    if(ok){
        API::setColor(y00, SIZE - x00 - 1, 'G');
    }
    return dir;
}

int getPath(int x, int y, int xx, int yy, int direction, bool ok=false) {
    if(x == xx && y == yy)
        return direction;
    vector<vector<int>> queue;
    queue.push_back({x, y});
    vector<vector<bool>> isVisited2(SIZE, vector<bool> (SIZE, 0));
    isVisited2[x][y] = true;
    pair<int, int> parent[SIZE][SIZE];
    for(auto & i : parent)
        for(auto & j : i)
            j = {-1, -1};
    while(!queue.empty()){
        vector<int> current = queue[0];
        queue.erase(queue.begin());
        int x1 = current[0];
        int y1 = current[1];
        for(pair<int, int> n : neighbors[x1][y1]){
            int nextX = n.first;
            int nextY = n.second;
            if(isVisited2[nextX][nextY]) continue;
            isVisited2[nextX][nextY] = true;
            queue.push_back({nextX, nextY});
            parent[nextX][nextY] = {x1, y1};
        }
    }
    assert(isVisited2[xx][yy]);
    return goToStartPoint(x, y, xx, yy, direction, parent, ok);
}
