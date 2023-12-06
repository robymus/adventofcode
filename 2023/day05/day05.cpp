#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <regex>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

struct range_t {
    range_t(long _start, long _end): range_t(_start, _end, 0) {}

    range_t(long _start, long _end, long _offset) {
        start = _start;
        end = _end;
        offset = _offset;
    }

    long start;   // first position in source range
    long end;     // last position in source range
    long offset;  // offset to add when converting
};

typedef vector<range_t> map_t;

struct input_t {
    vector<long> seeds;
    vector<map_t> maps;
};

input_t ReadInput(const string &inputFile) {
    input_t input;

    ifstream inputF(inputFile);
    string line;
    map_t *last_map = nullptr;
    while (getline(inputF, line)) {
        const static regex seedsLine(R"(seeds: (.*))");
        const static regex mapStart(R"(.*map:)");
        if (line.empty()) continue;
        smatch match;
        if (regex_match(line, match, seedsLine)) {
            istringstream i(match[1]);
            input.seeds.insert(input.seeds.end(),
                               istream_iterator<long>(i),
                               istream_iterator<long>());
        }
        else if (regex_match(line, match, mapStart)) {
            last_map = &input.maps.emplace_back();
        }
        else if (last_map != nullptr) {
            istringstream i(line);
            long dest_start, source_start, range;
            i >> dest_start >> source_start >> range;
            last_map->emplace_back(source_start, source_start + range - 1, dest_start - source_start);
        }
    }
    return input;
}

void Part1(input_t& input) {
    long min_location = -1;
    for (auto x : input.seeds) {
        for (auto &m : input.maps) {
            for (auto &c : m) {
                if (x >= c.start && x <= c.end) {
                    x += c.offset;
                    break;
                }
            }
        }
        if (min_location == -1 || x < min_location) min_location = x;
    }
    cout << "Part1: " << min_location << endl;
}

// sort ranges by start and possibly join adjacent/overlapping
void sortAndJoin(vector<range_t> &v)
{
    sort(v.begin(), v.end(), [](auto &a, auto &b) {
        return a.start == b.start ? a.end < b.end : a.start < b.start;
    });
    for (int i = 1; i < v.size(); i++) {
        if (v[i].start <= v[i-1].end+1) {
            v[i-1].end = v[i].end;
            v.erase(v.begin()+i);
        }
    }
}

void Part2(input_t& input) {
    vector<range_t> in;
    vector<range_t> out;
    // convert input seeds into vector of ranges
    for (int i = 0; i < input.seeds.size(); i += 2) {
        in.emplace_back(input.seeds[i], input.seeds[i] + input.seeds[i+1] - 1);
    }
    sortAndJoin(in);
    // sort each map by range end (there are no overlaps here, so it's also sorted by start)
    for (auto &m : input.maps) {
        sort(m.begin(), m.end(), [](auto &a, auto &b) { return a.end < b.end; });
    }
    // convert all ranges through all maps, with possible splitting
    for (auto &m : input.maps) {
        for (auto rangeIn : in) {
            auto conv = m.begin();
            long idx = rangeIn.start;
            while (idx <= rangeIn.end) {
                while (conv != m.end() && conv->end < idx) conv++;
                // conv is the next possible converter
                if (conv == m.end() || conv->start > rangeIn.end) {
                    // no more converting to be done - output rest
                    out.emplace_back(idx, rangeIn.end);
                    break;
                }
                if (conv->start > idx) {
                    out.emplace_back(idx, conv->start - 1);
                    idx = conv->start;
                }
                // we are at the start of the conversion - convert until we can
                long lastChanged = min(rangeIn.end, conv->end);
                out.emplace_back(idx + conv->offset, lastChanged + conv->offset);
                // go to next converter and continue (if this was not the end)
                idx = lastChanged + 1;
                conv++;
            }
        }
        // move from out to in, sort and reset
        in = std::move(out);
        out.clear();
        sortAndJoin(in);
    }
    long min_location = in[0].start;
    cout << "Part2: " << min_location << endl;
}

int main(int argc, char *argv[]) {
    auto input = ReadInput("input.txt");
    Part1(input);
    Part2(input);
    return 0;
}
