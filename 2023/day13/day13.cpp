#include <chrono>
#include <utility>
#include <fstream>
#include <iostream>
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

typedef vector<string> puzzle_t;
typedef vector<puzzle_t> input_t;

input_t ReadInput(const string &inputFile) {
    input_t input;

    ifstream inputF(inputFile);
    string line;
    puzzle_t puzzle;
    while (getline(inputF, line)) {
        if (line.empty() && !puzzle.empty()) {
            input.push_back(std::move(puzzle));
        }
        else {
            puzzle.push_back(line);
        }
    }
    if (!puzzle.empty()) {
        input.push_back(std::move(puzzle));
    }
    return input;
}

int findSym(vector<int> &v, int except = 0)
{
    vector<int> sym;
    int n = (int)v.size();
    for (int i = 1; i < n; i++) {
        if (i == except) continue;
        bool match = true;
        for (int j = 0; match && i-j-1 >= 0 && i+j < n; j++) {
            match = v[i-j-1] == v[i+j];
        }
        if (match) return i;
    }
    return 0;
}

// 59us (Release)
void Part1(input_t& input) {
    int sum = 0;
    for (auto &p : input) {
        vector<int> h(p[0].length()), v(p.size());
        for (int y = 0; y < p.size(); y++) {
            for (int x = 0; x < p[0].size(); x++) {
                int b = p[y][x] == '#' ? 1 : 0;
                v[y] |= b << x;
                h[x] |= b << y;
            }
        }
        sum += findSym(h) + 100*findSym(v);
    }
    cout << "Part1: " << sum << endl;
}

// Part 1 is very fast, let's just try flipping every single bit, and then we are still very fast :)
// 352us (Release)
void Part2(input_t& input) {
    int sum = 0;
    for (auto &p : input) {
        vector<int> h(p[0].length()), v(p.size());
        for (int y = 0; y < p.size(); y++) {
            for (int x = 0; x < p[0].size(); x++) {
                int b = p[y][x] == '#' ? 1 : 0;
                v[y] |= b << x;
                h[x] |= b << y;
            }
        }
        int ov = findSym(v);
        int oh = findSym(h);
        bool found = false;
        for (int y = 0; !found && y < p.size(); y++) {
            for (int x = 0; !found && x < p[0].size(); x++) {
                v[y] ^= 1 << x;
                h[x] ^= 1 << y;
                int nv = findSym(v, ov);
                int nh = findSym(h, oh);
                v[y] ^= 1 << x;
                h[x] ^= 1 << y;
                if (nv > 0 || nh > 0) {
                    found = true;
                    sum += nh + 100*nv;
                }
            }
        }
    }
    cout << "Part2: " << sum << endl;
}

int main(int argc, char *argv[]) {
    auto input = ReadInput("input.txt");
    benchmark(Part1, input);
    benchmark(Part2, input);
    return 0;
}
