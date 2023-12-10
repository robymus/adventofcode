#include <chrono>
#include <utility>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
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

struct input_t {
    size_t w = 0;
    size_t h = 0;
    size_t sx = 0;
    size_t sy = 0;
    vector<string> map; // to not handle going out of the map, we just add extra *-s on the outer edge
};

// type defining attributes of moving to a specific direction;
struct direction_t {
    int dx;
    int dy;
    string from;    // possible starting characters
    string to;      // possible ending characters
};

vector<direction_t> direction {
        {.dx = +1, .dy = 0, .from = "SFL-", .to = "S-7J"},
        {.dx = -1, .dy = 0, .from = "S-7J", .to = "SFL-"},
        {.dx = 0, .dy = +1, .from = "SF7|", .to = "S|LJ"},
        {.dx = 0, .dy = -1, .from = "S|LJ", .to = "SF7|"},
};

input_t ReadInput(const string &inputFile) {
    input_t input;
    ifstream inputF(inputFile);
    string line;
    while (getline(inputF, line)) {
        if (input.h == 0) {
            // first line
            input.w = line.size() + 2;
            input.h = 1;
            input.map.emplace_back(input.w, '*');
        }
        input.map.push_back("*" + line + "*");
        auto pos = line.find('S');
        if (pos != string::npos) {
            input.sx = pos + 1;
            input.sy = input.h;
        }
        input.h++;
    }
    input.h++;
    input.map.emplace_back(input.w, '*');
    return input;
}

void Part12(input_t& input) {
    int loopLength;
    bool found = false;
    // a new map, where only the path is drawn
    vector<string> pathMap(input.h);

    // start out from S in all directions
    for (auto &dir0: direction) {
        size_t x = input.sx + dir0.dx;
        size_t y = input.sy + dir0.dy;
        if (dir0.to.find(input.map[y][x]) != string::npos) {
            // reset pathMap
            fill(pathMap.begin(), pathMap.end(), string(input.w, ' '));
            // valid starting direction
            loopLength = 1;
            size_t lastX = input.sx;
            size_t lastY = input.sy;
            bool moved = true;
            while (moved && !found) {
                moved = false;
                for (auto &dir : direction) {
                    size_t nx = x + dir.dx;
                    size_t ny = y + dir.dy;
                    if (nx == lastX && ny == lastY) continue; // do not go backwards
                    if (dir.from.find(input.map[y][x]) == string::npos ||
                        dir.to.find(input.map[ny][nx]) == string::npos) continue; // illegal move
                    // legal move - go
                    lastX = x;
                    lastY = y;
                    x = nx;
                    y = ny;
                    loopLength++;
                    moved = true;
                    found = (x == input.sx && y == input.sy);
                    pathMap[ny][nx] = input.map[ny][nx];
                    // replace S in pathMap with a quasi proper symbol, based on dir0 and dir
                    if (found) {
                        char s;
                        if (dir0.dy == 0 && dir.dy == 0) {
                            s = '-';
                        } else if (dir0.dx == 0 && dir.dx == 0) {
                            s = '|';
                        } else if (dir0.dy == 1 ||
                                   dir.dy == -1) { // we either started down or ended up - bottom is covered
                            s = 'F'; // or maybe 7, but for counting area, it doesn't matter
                        } else { // top is covered
                            s = 'L'; // or maybe J, but for counting area, it doesn't matter
                        }

                        pathMap[input.sy][input.sx] = s;
                    }
                }
            }
        }
        if (found) break;
    }

    cout << "Part1: " <<  (loopLength+1)/2 << endl;

    // count interior area based on pathMap
    // we are drawing two scanning lines at the top and bottom of the character
    int area = 0;
    for (auto &line : pathMap) {
        bool insideTop = false;
        bool insideBottom = false;
        for (auto c : line) {
            switch (c) {
                case '|':
                    insideTop = !insideTop;
                    insideBottom = !insideBottom;
                    break;
                case 'F':
                case '7':
                    insideBottom = !insideBottom;
                    break;
                case 'L':
                case 'J':
                    insideTop = !insideTop;
                    break;
                default:
                    break;
            }
            if (insideTop && insideBottom && c == ' ') {
                area++;
            }
            else {
            }
        }
    }

    cout << "Part2: " << area << endl;

}

int main(int argc, char *argv[]) {
    auto input = ReadInput("input.txt");
    benchmark(Part12, input);
    return 0;
}
