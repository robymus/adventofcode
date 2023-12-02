#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

typedef map<string, int> round_t;
typedef vector<round_t> game_t;
typedef map<int, game_t> input_t;

input_t ReadInput(char *inputFile) {
    input_t r;

    ifstream input(inputFile);
    string line;
    while (getline(input, line)) {
        if (line.empty()) continue;
        const static regex lineFormat(R"(Game (\d+): (.*))");
        smatch match;
        if (!regex_match(line, match, lineFormat)) continue;
        int gameNumber = stoi(match[1]);
        game_t game;
        stringstream gameStr(match[2]);
        string roundStr;
        while (getline(gameStr, roundStr, ';')) {
            round_t round;
            stringstream roundStream(roundStr);
            string numColorStr;
            while (getline(roundStream, numColorStr, ',')) {
                const static regex numColorFormat(R"(\s*(\d+) ([a-z]*)\s*)");
                smatch m;
                if (!regex_match(numColorStr, m, numColorFormat)) continue;
                round[m[2]] = stoi(m[1]);
            }
            game.push_back(round);
        }
        r[gameNumber] = game;
    }
    return r;
}

void Part1(const input_t& input) {
    int sum = 0;
    for (auto g : input) {
        auto game = g.second;
        bool possible = true;
        for (auto round : game) {
            if (round.contains("red") && round["red"] > 12) possible = false;
            if (round.contains("green") && round["green"] > 13) possible = false;
            if (round.contains("blue") && round["blue"] > 14) possible = false;
        }
        if (possible) sum += g.first;
    }
    cout << "Part1: " << sum << endl;
}

void Part2(const input_t& input) {
    int sum = 0;
    for (auto g : input) {
        auto game = g.second;
        int min_r(0), min_g(0), min_b(0);
        for (auto round : game) {
            if (round.contains("red")) min_r = max(min_r, round["red"]);
            if (round.contains("green")) min_g = max(min_g, round["green"]);
            if (round.contains("blue")) min_b = max(min_b, round["blue"]);
        }
        sum += min_r * min_g * min_b;
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