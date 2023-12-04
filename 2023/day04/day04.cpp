#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <regex>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

struct card_t {
    vector<int> winning_numbers;
    vector<int> your_numbers;
};

typedef map<int, card_t> input_t;

input_t ReadInput(char *inputFile) {
    input_t input;

    ifstream inputF(inputFile);
    string line;
    while (getline(inputF, line)) {
        const static regex lineFormat(R"(Card +(\d+):([^|]*)\|([^|]*))");
        smatch match;
        if (!regex_match(line, match, lineFormat)) continue;
        istringstream winningStr(match[2]);
        istringstream yourStr(match[3]);
        card_t card;
        card.winning_numbers.insert(card.winning_numbers.end(),
                                    istream_iterator<int>(winningStr),
                                    istream_iterator<int>()
        );
        card.your_numbers.insert(card.your_numbers.end(),
                                 istream_iterator<int>(yourStr),
                                 istream_iterator<int>()
        );
        input[stoi(match[1])] = card;
    }

    return input;
}

void Part1(input_t& input) {
    int sum = 0;
    for (auto e : input) {
        unordered_set winningNumbers(e.second.winning_numbers.begin(), e.second.winning_numbers.end());
        int match = 0;
        for (auto x : e.second.your_numbers) {
            if (winningNumbers.contains(x)) match++;
        }
        if (match > 0) sum += 1 << (match-1);
    }
    cout << "Part1: " << sum << endl;
}

void Part2(input_t& input) {
    int totalCount = 0;
    vector<int> cardCount(input.size(), 1); // how many copies of each card do we have (starts at 1)
    int idx = 0;
    for (auto e : input) {
        int count = cardCount[idx++];
        totalCount += count;
        unordered_set winningNumbers(e.second.winning_numbers.begin(), e.second.winning_numbers.end());
        int match = 0;
        for (auto x : e.second.your_numbers) {
            if (winningNumbers.contains(x)) match++;
        }
        for (int i = 0; i < match && idx+i < cardCount.size(); i++) {
            cardCount[idx+i] += count;
        }
    }
    cout << "Part2: " << totalCount << endl;
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