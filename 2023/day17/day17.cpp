#include <chrono>
#include <utility>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;
using namespace std::chrono;

#define duration(a) std::chrono::duration_cast<std::chrono::nanoseconds>(a).count()

template<typename F, typename... Args>
void benchmark(F func, Args&&... args){
    high_resolution_clock::time_point t0=high_resolution_clock::now();
    func(std::forward<Args>(args)...);
    duration<double, std::micro> t = high_resolution_clock::now() - t0;
    cout << "[" << t << "]" << endl;
}

typedef vector<vector<int>> input_t;

input_t ReadInput(const string &inputFile) {
    input_t input;

    ifstream inputF(inputFile);
    string line;
    while (getline(inputF, line)) {
        vector<int> l;
        for (auto c : line) {
            l.push_back(c - '0');
        }
        input.push_back(l);
    }
    return input;
}

typedef vector<vector<int>> dist_t;

// dijkstra path finding, from start to all
dist_t dijkstra(input_t &input, int startX, int startY)
{
    int w = (int)input[0].size();
    int h = (int)input.size();
    typedef tuple<int, int, int> pqe;
    constexpr int INF = numeric_limits<int>::max();
    auto comparator = [](pqe a, pqe b) -> bool { return get<2>(a) > get<2>(b); };
    priority_queue<pqe, vector<pqe>, decltype(comparator)> q(comparator);

    dist_t dist;
    for (int y = 0; y < h; y++) {
        vector<int> d;
        for (int x = 0; x < w; x++) {
            bool start = x == startX && y == startY;
            d.push_back(start ? 0 : INF);
            q.emplace(x, y, start ? 0 : INF);
        }
        dist.push_back(std::move(d));
    }

    while (!q.empty()) {
        auto [x, y, d] = q.top();
        q.pop();
        if (d == INF || dist[y][x] != d) continue; // out of date element in queue
        const static vector<pair<int, int>> dirs { {-1, 0}, {+1, 0}, {0, -1}, {0, +1} };
        for (auto [dx, dy] : dirs) {
            int nx = x+dx, ny = y+dy;
            if (nx < 0 || ny < 0 || nx >= w || ny >= h) continue;
            int alt = d + input[ny][nx];
            if (alt < dist[ny][nx]) {
                dist[ny][nx] = alt;
                q.emplace(nx, ny, alt);
            }
        }
    }

    return dist;
}

constexpr int RIGHT = 0;
constexpr int DOWN = 1;
constexpr int LEFT = 2;
constexpr int UP = 3;

const vector<pair<int, int>> dxdy = { {1, 0}, {0, 1}, {-1, 0}, {0, -1}};

struct state_t
{
    int x;
    int y;
    int dir;
    int cnt;
};

int stateToInt(state_t s)
{
    return s.x << 16 | s.y << 8 | s.dir << 4 | s.cnt;
}

state_t stateFromInt(int state)
{
    return {
        .x = state >> 16 & 0xff,
        .y = state >> 8 & 0xff,
        .dir = state >> 4 & 0xf,
        .cnt = state & 0xf
    };
}

// returns length of path
int AStar(input_t &input, dist_t heuristics, state_t start, int goalX, int goalY,
          const function<vector<state_t>(const state_t&)> &findNeighbours)
{
    typedef tuple<state_t, int> pqe;
    constexpr int INF = numeric_limits<int>::max();
    auto comparator = [](pqe a, pqe b) -> bool { return get<1>(a) > get<1>(b); };
    priority_queue<pqe, vector<pqe>, decltype(comparator)> open(comparator);

    unordered_map<int, int> from;
    unordered_map<int, int> g;
    unordered_map<int, int> f;

    int si = stateToInt(start);
    g[si] = 0;
    f[si] = heuristics[start.y][start.x];
    open.emplace(start, heuristics[start.y][start.x]);

    int nodesExpanded;
    bool found = false;
    state_t traceState{.x=0, .y=0, .dir=0, .cnt=0};
    while (!open.empty()) {
        auto [current, d] = open.top();
        open.pop();
        int ci = stateToInt(current);
        if (!f.contains(ci) || f[ci] != d) continue; // out of date element in queue
        nodesExpanded++;
        if (current.x == goalX && current.y == goalY) {
            found = true;
            traceState = current;
            break;
        }
        vector<state_t> neighbours = findNeighbours(current);
        for (auto n : neighbours) {
            int ni = stateToInt(n);
            auto tg = g[ci] + input[n.y][n.x];
            if (!g.contains(ni) || tg < g[ni]) {
                from[ni] = ci;
                g[ni] = tg;
                f[ni] = tg + heuristics[n.y][n.x];
                open.emplace(n, f[ni]);
            }
        }
    }

    cout << "A* nodes expanded: " << nodesExpanded << endl;

    if (!found) return -1; // should not happen

    // trace back and add distance
    int dist = 0;
    while (traceState.x != start.x || traceState.y != start.y) {
        dist += input[traceState.y][traceState.x];
        traceState = stateFromInt(from[stateToInt(traceState)]);
    }

    return dist;
}

// 143.566ms, 116600 A* nodes
void Part1(input_t& input) {
    int w = (int)input[0].size();
    int h = (int)input.size();
    // calculate a heuristics by calculating the shortest distances from ending point
    dist_t heuristics(dijkstra(input, w-1, h-1));

    int dist = AStar(input, heuristics, {.x = 0, .y = 0, .dir=RIGHT, .cnt=0}, w-1, h-1,
                     [w,h](const state_t &current) -> vector<state_t> {
                         vector<state_t> neighbours;
                         if (current.cnt < 3) {
                             // straight
                             auto [dx, dy] = dxdy[current.dir];
                             int nx = current.x + dx, ny = current.y + dy;
                             if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                                 neighbours.push_back({.x=nx, .y=ny, .dir=current.dir, .cnt=current.cnt + 1});
                             }
                         }
                         for (int dd: {1, 3}) {
                             int nd = (current.dir + dd) % 4;
                             auto [dx, dy] = dxdy[nd];
                             int nx = current.x + dx, ny = current.y + dy;
                             if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                                 neighbours.push_back({.x=nx, .y=ny, .dir=nd, .cnt=1});
                             }
                         }
                         return neighbours;
                     });

    cout << "Part1: " << dist << endl;
}

// 980.546ms, 602574 A* nodes
void Part2(input_t& input) {
    int w = (int)input[0].size();
    int h = (int)input.size();
    // calculate a heuristics by calculating the shortest distances from ending point
    dist_t heuristics(dijkstra(input, w-1, h-1));

    int dist = AStar(input, heuristics, {.x = 0, .y = 0, .dir=RIGHT, .cnt=0}, w-1, h-1,
                     [w,h](const state_t &current) -> vector<state_t> {
                         vector<state_t> neighbours;
                         if (current.cnt > 0 && current.cnt < 4) { // >0 to work around our initial condition
                             // must go straight
                             auto [dx, dy] = dxdy[current.dir];
                             int nx = current.x + dx, ny = current.y + dy;
                             if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                                 neighbours.push_back({.x=nx, .y=ny, .dir=current.dir, .cnt=current.cnt + 1});
                             }
                         }
                         else {
                             if (current.cnt < 10) {
                                 // straight
                                 auto [dx, dy] = dxdy[current.dir];
                                 int nx = current.x + dx, ny = current.y + dy;
                                 if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                                     neighbours.push_back({.x=nx, .y=ny, .dir=current.dir, .cnt=current.cnt + 1});
                                 }
                             }
                             for (int dd: {1, 3}) {
                                 int nd = (current.dir + dd) % 4;
                                 auto [dx, dy] = dxdy[nd];
                                 int nx = current.x + dx, ny = current.y + dy;
                                 if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                                     neighbours.push_back({.x=nx, .y=ny, .dir=nd, .cnt=1});
                                 }
                             }
                         }
                         return neighbours;
                     });

    cout << "Part2: " << dist << endl;
}

int main(int argc, char *argv[]) {
    auto input = ReadInput("input.txt");
    benchmark(Part1, input);
    benchmark(Part2, input);
    return 0;
}
