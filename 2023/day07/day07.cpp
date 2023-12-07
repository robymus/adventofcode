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

struct hand_t {
    hand_t(string &&_hand, long _bid) : hand(_hand), bid(_bid) {}

    string hand;
    long bid;

    // filled during preprocessing
    string sortable;
    string sortable2;
    int type = 0;
    int type2 = 0;
};

typedef vector<hand_t> input_t;

input_t ReadInput(const string &inputFile) {
    input_t input;

    ifstream inputF(inputFile);
    string line;
    while (getline(inputF, line)) {
        const static regex lineFormat(R"(([^ ]+) ([0-9]+))");
        smatch match;
        if (regex_match(line, match, lineFormat)) {
            input.emplace_back(match[1].str(), stol(match[2]));
        }
    }
    return input;
}

// convert face cards to 'a', 'b', ...
void CreateSortable(hand_t &h) {
    ostringstream sortable;
    ostringstream sortable2;
    const static string faceCards = "TJQKA";
    for (auto c : h.hand) {
        auto idx = faceCards.find(c);
        char co = idx == string::npos ? c : (char)('a' + idx);
        sortable << co;
        sortable2 << (c == 'J' ? '1' : co);
    }
    h.sortable = sortable.str();
    h.sortable2 = sortable2.str();
}

void FindHandType(hand_t &h) {
    // count number of each cards
    map<char, int> tally;
    for (char c : h.hand) {
        if (tally.contains(c)) {
            tally[c]++;
        } else {
            tally[c] = 1;
        }
    }
    // created sorted list of counters
    vector<int> counters;
    int numJ = 0;
    vector<int> countersNoJ;
    for (auto t : tally) {
        counters.push_back(t.second);
        if (t.first == 'J') {
            numJ = t.second;
        }
        else {
            countersNoJ.push_back(t.second);
        }
    }
    sort(counters.begin(), counters.end(), greater());
    sort(countersNoJ.begin(), countersNoJ.end(), greater());
    while (counters.size() < 2) counters.push_back(0);
    while (countersNoJ.size() < 2) countersNoJ.push_back(0);
    // possible list of types, counter[0], counter[1], value
    const static vector<tuple<int, int, int>>types {
        tuple(5, 0, 6),
        tuple(4, 0, 5),
        tuple(3, 2, 4),
        tuple(3, 0, 3),
        tuple(2, 2, 2),
        tuple(2, 0, 1),
    };
    // Part1: find the highest type
    h.type = 0;
    for (auto [c0, c1, t] : types) {
        if (counters[0] >= c0 && counters[1] >= c1) {
            h.type = t;
            break;
        }
    }
    // Part2: find the highest possible type
    h.type2 = 0;
    for (auto [c0, c1, t] : types) {
        int j0 = 0;
        if (countersNoJ[0] < c0) {
            j0 = c0 - countersNoJ[0]; // number of J needed for counter0
        }
        // check if possible
        if (j0 <= numJ && countersNoJ[1] + numJ - j0 >= c1) {
            h.type2 = t;
            break;
        }
    }
}

bool HandSort(const hand_t &a, const hand_t &b) {
    return a.type == b.type ? a.sortable < b.sortable : a.type < b.type;
}

bool HandSort2(const hand_t &a, const hand_t &b) {
    return a.type2 == b.type2 ? a.sortable2 < b.sortable2 : a.type2 < b.type2;
}

void PreprocessInput(input_t& input) {
    for (auto &h : input) {
        CreateSortable(h);
        FindHandType(h);
    }
    cout << "Preprocess" << endl;
}

void Part1(input_t& input) {
    sort(input.begin(), input.end(), &HandSort);
    long sum = 0;
    for (int i = 0; i < input.size(); i++) {
        sum += input[i].bid * (i+1);
    }
    cout << "Part1: " << sum << endl;
}

void Part2(input_t& input) {
    sort(input.begin(), input.end(), &HandSort2);
    long sum = 0;
    for (int i = 0; i < input.size(); i++) {
        sum += input[i].bid * (i+1);
    }
    cout << "Part2: " << sum << endl;
}

int main(int argc, char *argv[]) {
    auto input = ReadInput("input.txt");
    benchmark(PreprocessInput, input);
    benchmark(Part1, input);
    benchmark(Part2, input);
    return 0;
}
