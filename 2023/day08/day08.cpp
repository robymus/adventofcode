#include <chrono>
#include <utility>
#include <fstream>
#include <iostream>
#include <numeric>
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
    string path;
    map<string, pair<string, string>> graph;
};

input_t ReadInput(const string &inputFile) {
    input_t input;

    ifstream inputF(inputFile);
    getline(inputF, input.path);
    string line;
    while (getline(inputF, line)) {
        const static regex lineFormat(R"((...) = \((...), (...)\))");
        smatch match;
        if (regex_match(line, match, lineFormat)) {
            input.graph[match[1]] = pair(match[2], match[3]);
        }
    }
    return input;
}

void Part1(input_t& input) {
    long steps = 0;
    size_t pathPos = 0;
    string pos = "AAA";
    while (pos != "ZZZ") {
        auto node = input.graph[pos];
        pos = input.path[pathPos] == 'L' ? node.first : node.second;
        pathPos = (pathPos + 1) % input.path.size();
        steps++;
    }

    cout << "Part1: " <<  steps << endl;
}

void Part2(input_t& input) {
    // find starting positions
    vector<string> allPos;
    for (auto &node : input.graph) {
        if (node.first[2] == 'A') allPos.push_back(node.first);
    }
    // for each starting position, try to find a loop
    vector<pair<long, long>> loops; // first ending position, length of loop
    for (auto pos : allPos) {
        map<pair<string, size_t>, long> index;
        vector<long> endPos;
        long steps = 0;
        size_t pathPos = 0;
        while (!index.contains(pair(pos, pathPos))) {
            index[pair(pos, pathPos)] = steps;
            if (pos[2] == 'Z') endPos.push_back(steps);
            auto node = input.graph[pos];
            pos = input.path[pathPos] == 'L' ? node.first : node.second;
            pathPos = (pathPos + 1) % input.path.size();
            steps++;
        }
        long loopStart = index[pair(pos, pathPos)];
        long loopLength = steps - loopStart;
        // Note: there is only 1 ending point inside each loop, this simplifies things
        loops.emplace_back(endPos[0], loopLength);
        cout << "Loop @" << endPos[0] << " len=" << loopLength << " x=" << loopStart << " l=" << endPos.size() << endl;
    }
    // The input is so nice, loopStart == loopLength for all loops - so let's just LCM all loop lengths
    long s = 1;
    for (auto l : loops) {
        s = lcm(s, l.second);
    }

    cout << "Part2: " << s << endl;
}

int main(int argc, char *argv[]) {
    auto input = ReadInput("input.txt");
    benchmark(Part1, input);
    benchmark(Part2, input);
    return 0;
}
