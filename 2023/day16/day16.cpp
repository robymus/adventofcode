#include <chrono>
#include <utility>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <queue>
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

typedef vector<string> input_t;

input_t ReadInput(const string &inputFile) {
    input_t input;

    ifstream inputF(inputFile);
    string line;
    while (getline(inputF, line)) {
        input.push_back(line);
    }
    return input;
}

constexpr int LEFT = 0;
constexpr int RIGHT = 1;
constexpr int UP = 2;
constexpr int DOWN = 3;
constexpr int REVERSE = 1; // ^ by this

// which direction is light coming from / going to (bitmap, 1<<dir)
struct loc_t {
    explicit loc_t(char content) {
        this->content = content;
        lightFrom = 0;
        lightTo = 0;
    }
    char content;
    char lightFrom;
    char lightTo;
};

typedef vector<vector<loc_t>> map_t;

map_t convertInput(input_t &input) {
    map_t map;
    for (auto &line : input) {
        vector<loc_t> mapLine;
        for (auto &c : line) {
            mapLine.emplace_back(c);
        }
        map.push_back(std::move(mapLine));
    }
    return map;
}

pair<int, int> dirToDxDy[] {
    {-1, 0},
    {+1, 0},
    {0, -1},
    {0, +1},
};

map<pair<char, int>, vector<int>> obstacleAndDirToDir {
    { {'\\', RIGHT}, {DOWN} },
    { {'\\', LEFT}, {UP} },
    { {'\\', UP}, {LEFT} },
    { {'\\', DOWN}, {RIGHT} },

    { {'/', RIGHT}, {UP} },
    { {'/', LEFT}, {DOWN} },
    { {'/', UP}, {RIGHT} },
    { {'/', DOWN}, {LEFT} },

    { {'|', LEFT}, {UP, DOWN} },
    { {'|', RIGHT}, {UP, DOWN} },
    { {'|', UP}, {UP} },
    { {'|', DOWN}, {DOWN} },

    { {'-', UP}, {LEFT, RIGHT} },
    { {'-', DOWN}, {LEFT, RIGHT} },
    { {'-', LEFT}, {LEFT} },
    { {'-', RIGHT}, {RIGHT} },

    { {'.', UP}, {UP} },
    { {'.', DOWN}, {DOWN} },
    { {'.', LEFT}, {LEFT} },
    { {'.', RIGHT}, {RIGHT} },
};

void resetLightMap(map_t &map) {
    for (auto &ll : map) {
        for (auto &l : ll) {
            l.lightFrom = 0;
            l.lightTo = 0;
        }
    }
}

// return number of cells energized by this run
int fillLightMap(map_t &map, int startX, int startY, int startDir) {
    int energized = 0;
    queue<tuple<int, int, int>> open;
    open.emplace(startX, startY, startDir);
    while (!open.empty()) {
        auto [x, y, lightDir] = open.front();
        open.pop();
        if (map[y][x].lightFrom & 1 << (lightDir ^ REVERSE)) continue; // light is there already
        if (!map[y][x].lightFrom) energized++;
        map[y][x].lightFrom |= 1 << (lightDir ^ REVERSE);
        pair<char, int> idx = make_pair(map[y][x].content, lightDir);
        if (!obstacleAndDirToDir.contains(idx)) continue;
        for (auto &to : obstacleAndDirToDir[idx]) {
            auto [dx, dy] = dirToDxDy[to];
            int nx = x+dx, ny = y+dy;
            if (nx < 0 || nx >= map[0].size() || ny < 0 || ny >= map.size()) continue; // out of bounds
            open.emplace(nx, ny, to);
            map[y][x].lightTo |= 1 << to;
        }
    }
    return energized;
}

// 0.429ms
void Part1(input_t& input) {
    auto map = convertInput(input);
    int energized = fillLightMap(map, 0, 0, RIGHT);
    cout << "Part1: " <<  energized << endl;
}

// Part 1 was fast enough, let's just bruteforce
// 63.822ms
void Part2(input_t& input) {
    auto map = convertInput(input);
    int maxEnergy = 0;
    int w = (int)map[0].size();
    int h = (int)map.size();
    for (int x = 0; x < w; x++) {
        resetLightMap(map);
        maxEnergy = max(maxEnergy, fillLightMap(map, x, 0, DOWN));
        resetLightMap(map);
        maxEnergy = max(maxEnergy, fillLightMap(map, x, h-1, UP));
    }
    for (int y = 0; y < h; y++) {
        resetLightMap(map);
        maxEnergy = max(maxEnergy, fillLightMap(map, 0, y, RIGHT));
        resetLightMap(map);
        maxEnergy = max(maxEnergy, fillLightMap(map, w-1, y, LEFT));
    }
    cout << "Part2: " << maxEnergy << endl;
}

int main(int argc, char *argv[]) {
    auto input = ReadInput("input.txt");
    benchmark(Part1, input);
    benchmark(Part2, input);
    return 0;
}
