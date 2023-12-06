#include <chrono>
#include <utility>
#include <cmath>
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
    duration<double, std::milli> t = high_resolution_clock::now() - t0;
    cout << "[" << t << " ms]" << endl;
}

struct input_t {
    size_t size = 0;
    vector<int> time;
    vector<int> distance;
    long part2time = 0;
    long part2distance = 0;
};

input_t ReadInput(const string &inputFile) {
    input_t input;

    ifstream inputF(inputFile);
    string line;
    while (getline(inputF, line)) {
        const static regex timeLine(R"(Time: (.*))");
        const static regex distanceLine(R"(Distance: (.*))");
        smatch match;
        if (regex_match(line, match, timeLine)) {
            istringstream i(match[1]);
            input.time.insert(input.time.end(),
                               istream_iterator<int>(i),
                               istream_iterator<int>());
            string s(match[1]);
            s.erase(remove(s.begin(), s.end(), ' '), s.end());
            input.part2time = stol(s);
        } else if (regex_match(line, match, distanceLine)) {
            istringstream i(match[1]);
            input.distance.insert(input.distance.end(),
                              istream_iterator<int>(i),
                              istream_iterator<int>());
            string s(match[1]);
            s.erase(remove(s.begin(), s.end(), ' '), s.end());
            input.part2distance = stol(s);
        }
    }
    input.size = input.time.size();
    return input;
}

void Part1(input_t& input) {
    int total = 1;
    for (size_t i = 0; i < input.size; i++) {
        int time = input.time[i];
        int distance = input.distance[i];
        int possible = 0;
        for (int b = 1; b < time; b++) {
            if (b * (time-b) > distance) possible++;
        }
        total *= possible;
    }
    cout << "Part1: " << total << endl;
}

void Part2BruteForce(input_t& input) {
    long possible = 0;
    for (long b = 1; b < input.part2time; b++) {
        if (b * (input.part2time-b) > input.part2distance) possible++;
    }
    cout << "Part2: " << possible << endl;
}

void Part2Math(input_t& input) {
    auto t = (long double)input.part2time;
    auto d = (long double)input.part2distance;
    auto _d= sqrt(t*t-4*d);

    // +2 to compensate for possible precision loss for doing floating point arithmetic
    long b0 = (long) ceil((t-_d)/2) + 2;
    long b1 = (long) floor((t+_d)/2) - 2;

    // we are near the edge, find the actual values
    while ((b0 * (input.part2time-b0) > input.part2distance)) b0--; // b0 is the first bad
    while ((b1 * (input.part2time-b1) > input.part2distance)) b1++; // b1 is the first bad

    long possible = b1-b0-1;
    cout << "Part2: " << possible << endl;
}

int main(int argc, char *argv[]) {
    auto input = ReadInput("input.txt");
    benchmark(Part1, input);
    benchmark(Part2BruteForce, input);
    benchmark(Part2Math, input);
    return 0;
}
