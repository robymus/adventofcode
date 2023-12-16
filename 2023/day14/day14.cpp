#include <bit>
#include <chrono>
#include <utility>
#include <fstream>
#include <iostream>
#include <string>
#include <queue>
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

// 0.069ms
void Part1(input_t& input) {
    // roll north and calculate weight
    long sum = 0;
    for (int y0 = 0; y0 < input.size(); y0++) {
        for (int x = 0; x < input[0].length(); x++) {
            if (input[y0][x] != 'O') continue;
            int y = y0;
            while (y > 0 && input[y-1][x] == '.') {
                input[y][x] = '.';
                y--;
                input[y][x] = 'O';
            }
            sum += (int)input.size() - y;
        }
    }
    cout << "Part1: " <<  sum << endl;
}

void rollX(input_t& m, int start, int end, int dir) {
    for (auto &line : m) {
        queue<int> empty;
        for (int x = start; x != end; x+=dir) {
            switch (line[x]) {
                case '#':
                    empty = {};
                    break;
                case '.':
                    empty.push(x);
                    break;
                case 'O':
                    if (!empty.empty()) {
                        line[empty.front()] = 'O';
                        line[x] = '.';
                        empty.pop();
                        empty.push(x);
                    }
            }
        }
    }
}

void rollY(input_t& m, int start, int end, int dir) {
    for (int x = 0; x < m[0].length(); x++) {
        queue<int> empty;
        for (int y = start; y != end; y+=dir) {
            switch (m[y][x]) {
                case '#':
                    empty = {};
                    break;
                case '.':
                    empty.push(y);
                    break;
                case 'O':
                    if (!empty.empty()) {
                        m[empty.front()][x] = 'O';
                        m[y][x] = '.';
                        empty.pop();
                        empty.push(y);
                    }
            }
        }
    }
}
void rollCycle(input_t& m) {
    int h = (int)m.size();
    int w = (int)m[0].length();
    rollY(m, 0, h, +1);
    rollX(m, 0, w, +1);
    rollY(m, h-1, -1, -1);
    rollX(m, w-1, -1, -1);
}

size_t myHash(input_t &m) {
    std::size_t h0 = 0;
    for (auto &line : m) {
        std::size_t h = std::hash<std::string>{}(line);
        h0 = rotl(h0, 1) ^ h;
    }
    return h0;
}

bool validate(input_t &starting, int iterations, input_t &now)
{
    input_t m = starting;
    for (int i = 0; i < iterations; i++) {
        rollCycle(m);
    }
    for (int y = 0; y < m.size(); y++) {
        if (m[y] != now[y]) return false;
    }
    return true;
}

// 350ms
void Part2(input_t& input) {
    input_t m = input; // make a copy for modifying
    unordered_map<size_t, int> found;
    int dest = 1000000000;
    bool skippedForward = false;
    int i = 0;
    while (i < dest) {
        rollCycle(m);
        i++;
        if (!skippedForward) {
            // try to skip forward by identifying a loop
            size_t h = myHash(m);
            if (found.contains(h)) {
                cout << "Loop candidate @" << i << endl;
                int lastI = found[h];
                if (validate(input, lastI, m)) {
                    int loopSize = i-lastI;
                    cout << "Valid loop @" << i << " size: " << loopSize << endl;
                    int left = dest-i;
                    left %= loopSize; // this is actual left
                    i = dest-left;
                    cout << "Jump to: " << i << endl;
                    skippedForward = true;
                }
            }
            else {
                found[h] = i;
            }
        }
    }
    // calculate north support beam load
    int load = 0;
    for (int y = 0, l = (int)m.size(); y < m.size(); y++, l--) {
        for (int x = 0; x < m[0].size(); x++) {
            if (m[y][x] == 'O') load += l;
        }
    }
    cout << "Part2: " << load << endl;
}

int main(int argc, char *argv[]) {
    auto input = ReadInput("input.txt");
    benchmark(Part1, input);
    benchmark(Part2, input);
    return 0;
}
