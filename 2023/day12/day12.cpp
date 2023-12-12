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

struct report_t {
    string pattern;
    vector<int> blocks;
};

typedef vector<report_t> input_t;

constexpr int max_strlen = 128;  // 104
constexpr int max_blockcnt = 32; // 30
constexpr int max_blocklen = 32; // 17

input_t ReadInput(const string &inputFile) {
    input_t input;

    ifstream inputF(inputFile);
    string line;
    while (getline(inputF, line)) {
        const static regex lineFormat(R"(([.#?]+) (.*))");
        smatch match;
        if (regex_match(line, match, lineFormat)) {
            report_t report;
            report.pattern = match[1];
            string blocks = match[2];
            replace(blocks.begin(), blocks.end(), ',', ' ');
            istringstream i(blocks);
            report.blocks.insert(report.blocks.end(),
                                 istream_iterator<int>(i),
                                 istream_iterator<int>());
            input.push_back(report);
        }
    }
    return input;
}

// check possible combinations via bruteforce (with minimal optimization: prefix matching)
int bruteforce(string &pattern, vector<int> &blocks) {
    size_t damagePosition = string::npos;
    vector<int> sureBlocks;
    bool inBlock = false;
    int blockSize;
    // calculate sure blocks, up until first ?
    for (size_t i = 0; damagePosition == string::npos && i < pattern.length(); i++) {
        switch (pattern[i]) {
            case '#':
                if (inBlock) {
                    blockSize++;
                } else {
                    inBlock = true;
                    blockSize = 1;
                }
                break;
            case '.':
                if (inBlock) {
                    sureBlocks.push_back(blockSize);
                    inBlock = false;
                }
                break;
            case '?':
                damagePosition = i;
                break;
        }
    }
    if (damagePosition == string::npos && inBlock) {
        sureBlocks.push_back(blockSize);
    }
    // check if everything in sureBlocks matches blocks
    int sureMatch = 0;
    while (sureMatch < sureBlocks.size() && sureMatch < blocks.size() && sureBlocks[sureMatch] == blocks[sureMatch]) {
        sureMatch++;
    }
    if (sureMatch != sureBlocks.size()) return 0; // not possible
    if (damagePosition == string::npos) {
        // no more damage
        return sureMatch == blocks.size() ? 1 : 0;
    } else {
        // there is a damage
        int p = 0;
        pattern[damagePosition] = '.';
        p += bruteforce(pattern, blocks);
        pattern[damagePosition] = '#';
        p += bruteforce(pattern, blocks);
        pattern[damagePosition] = '?';
        return p;
    }
}

// 295866us
void Part1(input_t& input) {
    long sum = 0;
    for (auto &report : input) {
        string pattern = report.pattern; // make a copy, it will be destroyed
        sum += bruteforce(pattern, report.blocks);
    }
    cout << "Part1: " <<  sum << endl;
}

// dynamic programming solution

long memo[max_strlen][max_blockcnt][max_blocklen];
string pattern_;
vector<int> blocks_;

void resetMemo() {
    fill_n(&memo[0][0][0], max_strlen * max_blockcnt * max_blocklen, -1);
}

long dp(size_t string_pos, size_t block_match, size_t open_block_size) {
    long m = memo[string_pos][block_match][open_block_size];
    if (m >= 0) return m;

    else if (string_pos == 0) {
        // at pos 0, all zero is possible, everything else is not possible
        m = (block_match == 0 && open_block_size == 0) ? 1 : 0;
    } else {
        // at a different position, let's see what do we have at current position and do the magic
        m = 0;
        auto c = pattern_[string_pos-1];
        // ? acts as both # and .
        if (c == '#' || c == '?') {
            // if we are at a #, the previous must a one smaller open block size
            if (open_block_size > 0) m += dp(string_pos - 1, block_match, open_block_size - 1);
        }
        if (c == '.' || c == '?') {
            // if we are at ., this can't be an open block
            if (open_block_size == 0) {
                // if we are not before all blocks
                if (block_match > 0) {
                    // either this . terminated an open block
                    m += dp(string_pos - 1, block_match - 1, blocks_[block_match - 1]);
                    // or not
                    m += dp(string_pos - 1, block_match, open_block_size);
                }
                // before all blocks
                else {
                    m += dp(string_pos - 1, block_match, open_block_size);
                }
            }
        }
    }

    memo[string_pos][block_match][open_block_size] = m;
    return m;
}

long solveDp(string &pattern, vector<int> &blocks)
{
    pattern_ = pattern + ".";
    blocks_ = blocks;
    resetMemo();
    return dp(pattern_.size(), blocks_.size(), 0);
}

// 250259us
void Part1Dp(input_t& input) {
    long sum = 0;
    for (auto &report : input) {
        sum += solveDp(report.pattern, report.blocks);
    }
    cout << "Part1 (DP): " <<  sum << endl;
}


// 273520us
void Part2(input_t& input) {
    long sum = 0;
    for (auto &report : input) {
        string pattern; // make a copy, it will be destroyed
        vector<int> blocks;
        for (int i = 0; i < 5; i++) {
            if (i > 0) pattern.append("?");
            pattern.append(report.pattern);
            blocks.insert(blocks.end(), report.blocks.begin(), report.blocks.end());
        }
        long s = solveDp(pattern, blocks);
        sum += s;
    }
    cout << "Part2: " << sum << endl;
}

int main(int argc, char *argv[]) {
    auto input = ReadInput("input.txt");
    benchmark(Part1, input);
    benchmark(Part1Dp, input);
    benchmark(Part2, input);
    return 0;
}
