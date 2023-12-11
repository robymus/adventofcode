#include <chrono>
#include <utility>
#include <fstream>
#include <iostream>
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

typedef long int_t;

struct input_t {
    size_t w = 0;
    size_t h = 0;
    vector<pair<int_t, int_t>> galaxies;
};
input_t ReadInput(const string &inputFile) {
    input_t input;

    ifstream inputF(inputFile);
    string line;
    while (getline(inputF, line)) {
        size_t pos = line.find('#', 0);
        while (pos != string::npos)
        {
            input.galaxies.emplace_back((int_t)pos, (int_t)input.h);
            pos = line.find('#', pos+1);
        }
        input.h++;
        input.w = line.length();
    }
    return input;
}

// calculate the total distances per coordinate
int_t sumDist(vector<int_t> &coords, int_t expansion)
{
    sort(coords.begin(), coords.end());
    int_t sum = 0;
    int_t before = 1;
    auto after = (int_t)coords.size()-1;
    for (size_t i = 1; i < coords.size(); i++) {
        // this segment (last .. current) participates before*after times
        // distance is current-last-1 * expansion + 1 -> this accounts for the expanding galaxy
        if (coords[i] != coords[i-1]) {
            sum += before * after * ((coords[i] - coords[i-1] - 1) * expansion + 1);
        }
        before++;
        after--;
    }
    return sum;
}

void Part12(input_t& input) {
    vector<int_t> xCoord;
    vector<int_t> yCoord;
    for (auto &g : input.galaxies) {
        xCoord.push_back(g.first);
        yCoord.push_back(g.second);
    }
    int_t totalDist1 = sumDist(xCoord, 2) + sumDist(yCoord, 2);
    int_t totalDist2 = sumDist(xCoord, 1000000) + sumDist(yCoord, 1000000);
    cout << "Part1: " <<  totalDist1 << endl;
    cout << "Part2: " <<  totalDist2 << endl;
}


int main(int argc, char *argv[]) {
    auto input = ReadInput("input.txt");
    benchmark(Part12, input);
    return 0;
}
