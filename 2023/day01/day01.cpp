#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<string> ReadInput(char *inputFile) {
    ifstream input(inputFile);
    vector<string> r;
    string buf;
    while (getline(input, buf)) {
        if (!buf.empty()) r.push_back(buf);
    }
    return r;
}

void part1(vector<string> lines) {
    int sum = 0;
    for (auto &line : lines) {
        bool isFirst = true;
        int first;
        int last;
        for (char c : line) {
            if (isdigit(c)) {
                if (isFirst) {
                    isFirst = false;
                    first = c-'0';
                }
                last = c-'0';
            }
        }
        sum += first * 10 + last;
    }
    cout << "Part1: " << sum << endl;
}

void part2(vector<string> lines) {
    string numbers[] {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
    int sum = 0;
    for (auto &line : lines) {
        size_t firstIndex = string::npos;
        int first;
        size_t lastIndex = string::npos;
        int last;
        for (int i = 0; i < 10; i++) {
            size_t idx;
            idx = line.find('0'+i);
            if (idx != string::npos && (firstIndex == string::npos || idx < firstIndex)) {
                firstIndex = idx;
                first = i;
            }
            if (i > 0) {
                idx = line.find(numbers[i - 1]);
                if (idx != string::npos && (firstIndex == string::npos || idx < firstIndex)) {
                    firstIndex = idx;
                    first = i;
                }
            }
            idx = line.rfind('0'+i);
            if (idx != string::npos && (lastIndex == string::npos || idx > lastIndex)) {
                lastIndex = idx;
                last = i;
            }
            if (i > 0) {
                idx = line.rfind(numbers[i - 1]);
                if (idx != string::npos && (lastIndex == string::npos || idx > lastIndex)) {
                    lastIndex = idx;
                    last = i;
                }
            }
        }
        sum += first * 10 + last;
    }
    cout << "Part2: " << sum << endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "input file required as argument" << endl;
        return 1;
    }
    auto lines = ReadInput(argv[1]);
    part1(lines);
    part2(lines);
    return 0;
}