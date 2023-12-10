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

struct input_t {
    vector<vector<long>> inputSeq;
    // preprocessed values
    size_t s = 0;
    vector<vector<long>> seqStack;
};


input_t ReadInput(const string &inputFile) {
    input_t input;

    ifstream inputF(inputFile);
    string line;
    while (getline(inputF, line)) {
        vector<long> seq;
        istringstream i(line);
        seq.insert(seq.end(),
                      istream_iterator<long>(i),
                      istream_iterator<long>());
        input.inputSeq.push_back(std::move(seq));
    }
    return input;
}

void Preprocess(input_t &input) {
    // input is nice, as all sequences have the same length - let's just add them all together
    input.s = input.inputSeq[0].size();
    auto s = input.s;
    vector<long> sum(s);
    for (int i = 0; i < s; i++) {
        sum[i] = 0;
        for (auto seq : input.inputSeq) {
            sum[i] += seq[i];
        }
    }
    // do the bruteforce diffing
    auto &seqStack = input.seqStack;
    seqStack.push_back(std::move(sum));
    bool allZero = false;
    size_t idx = 0;
    while (!allZero) {
        size_t next_s = s - idx - 1;
        allZero = true;
        vector<long> next(next_s);
        for (int i = 0; i < next_s; i++) {
            next[i] = seqStack[idx][i+1] - seqStack[idx][i];
            allZero &= next[i] == 0;
        }
        seqStack.push_back(std::move(next));
        idx++;
    }
}

void Part1(input_t& input) {
    // construct prediction
    long predict = 0;
    for (int i = 0; i < input.seqStack.size(); i++) {
        predict += input.seqStack[i][input.s-i-1];
    }

    cout << "Part1: " <<  predict << endl;
}

void Part2(input_t& input) {
    // construct prediction
    long predict = 0;
    for (int i = 0; i < input.seqStack.size(); i++) {
        predict -= input.seqStack[i][0] * (i%2 ? 1 : -1); // -1 ** i
    }

    cout << "Part2: " <<  predict << endl;
}

int main(int argc, char *argv[]) {
    auto input = ReadInput("input.txt");
    benchmark(Preprocess, input);
    benchmark(Part1, input);
    benchmark(Part2, input);
    return 0;
}
