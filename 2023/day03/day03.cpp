#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

struct input_t {
    int width{0};
    int height{0};
    vector<string> s;
};

input_t ReadInput(char *inputFile) {
    input_t input;

    ifstream inputF(inputFile);
    string line;
    while (getline(inputF, line)) {
        if (line.empty()) continue;
        input.s.push_back(line);
        input.width = (int)line.length();
        input.height++;
    }

    return input;
}

bool isSymbol(input_t& input, int x, int y) {
    if (x < 0 || y < 0 || x >= input.width || y >= input.height) return false;
    char c = input.s[y][x];
    return c != '.' && !isdigit(c);
}

void Part1(input_t& input) {
    int x{0}, y{0};
    int sum{0};
    // scan for numbers
    while (y < input.height) {
        if (isdigit(input.s[y][x])) {
            // start of a number
            int x1 = x;
            while (x1 < input.width && isdigit(input.s[y][x1+1])) x1++;
            bool hasSymbol = false;
            for (int yy=y-1; !hasSymbol && yy <= y+1; yy++) {
                for (int xx=x-1; !hasSymbol && xx <= x1+1; xx++) {
                    hasSymbol = isSymbol(input, xx, yy);
                }
            }
            if (hasSymbol) {
                sum += stoi(input.s[y].substr(x, x1-x+1));
            }
            x = x1+1;
            if (x == input.width) {
                x = 0;
                y++;
            }
        }
        else {
            x++;
            if (x == input.width) {
                x = 0;
                y++;
            }
        }
    }
    cout << "Part1: " << sum << endl;
}

void Part2(input_t& input) {
    map<pair<int, int>, vector<int>> gears;
    // find all gears first
    for (int y = 0; y < input.height; y++) {
        for (int x = 0; x < input.width; x++) {
            if (input.s[y][x] == '*') gears[make_pair(x, y)] = {};
        }
    }
    // find all numbers and assign to gears
    int x{0}, y{0};
    // scan for numbers
    while (y < input.height) {
        if (isdigit(input.s[y][x])) {
            // start of a number
            int x1 = x;
            while (x1 < input.width && isdigit(input.s[y][x1+1])) x1++;
            int val = stoi(input.s[y].substr(x, x1-x+1));
            // check if adjacent to any gears
            for (int yy=y-1; yy <= y+1; yy++) {
                for (int xx=x-1; xx <= x1+1; xx++) {
                    auto idx = make_pair(xx, yy);
                    if (gears.contains(idx)) gears[idx].push_back(val);
                }
            }
            x = x1+1;
            if (x == input.width) {
                x = 0;
                y++;
            }
        }
        else {
            x++;
            if (x == input.width) {
                x = 0;
                y++;
            }
        }
    }
    // add all gear ratios
    int sum{0};
    for (auto g : gears) {
        if (g.second.size() == 2) sum += g.second[0] * g.second[1];
    }
    cout << "Part2: " << sum << endl;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "input file required as argument" << endl;
        return 1;
    }
    auto input = ReadInput(argv[1]);
    Part1(input);
    Part2(input);
    return 0;
}