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

typedef vector<string> input_t;

input_t ReadInput(const string &inputFile) {
    input_t input;

    ifstream inputF(inputFile);
    string line;
    while (getline(inputF, line, ',')) {
        input.push_back(line);
    }
    return input;
}

int HASH(const string &s) {
    int h = 0;
    for (char c : s) {
        h = ((h + c) * 17) % 256;
    }
    return h;
}

// 0.05ms
void Part1(input_t& input) {
    int sum = 0;
    for (auto &s : input) {
        sum += HASH(s);
    }
    cout << "Part1: " <<  sum <<  endl;
}

struct parsed_t {
    explicit parsed_t(const string &s) {
        int h = 0;
        for (char c : s) {
            if (c == '-') {
                focal = 0;
                break;
            }
            else if (c == '=') {
                focal = s[s.length()-1] - '0';
                break;
            }
            h = ((h + c) * 17) % 256;
            label.push_back(c);
        }
        box = h;
    }

    string label;
    int box;
    int focal;
};

// 0.319ms
void Part2(input_t& input) {
    vector<pair<string, int>> boxes[256];
    for (auto &s: input) {
        parsed_t p(s);
        auto &box = boxes[p.box];
        bool replaced = false;
        // remove any matching lenses - eg. replace with focal 0
        for (auto &e : box) {
            if (e.second > 0 && e.first == p.label) {
                e.second = p.focal;
                replaced = true;
            }
        }
        // push new to the end if not 0 and was not replaced
        if (p.focal > 0 && !replaced) box.emplace_back(p.label, p.focal);
    }
    // calculate
    long sum = 0;
    int boxIdx = 1;
    for (auto &box : boxes) {
        int slotIdx = 1;
        for (auto &e : box) {
            if (e.second > 0) {
                sum += boxIdx * slotIdx * e.second;
                slotIdx++;
            }
        }
        boxIdx++;
    }
    cout << "Part2: " << sum << endl;
}

int main(int argc, char *argv[]) {
    auto input = ReadInput("input.txt");
    benchmark(Part1, input);
    benchmark(Part2, input);
    return 0;
}
