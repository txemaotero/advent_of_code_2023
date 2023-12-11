#include <algorithm>
#include <cassert>
#include <fstream>
#include <string>
#include <array>
#include <iostream>
#include <unordered_map>
#include "../utils.hpp"

enum HandType
{
    HIGH_CARD = 0,
    ONE_PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    FIVE_OF_A_KIND
};

static const std::unordered_map<char, int> card_values = {
    {'2', 1},
    {'3', 2},
    {'4', 3},
    {'5', 4},
    {'6', 5},
    {'7', 6},
    {'8', 7},
    {'9', 8},
    {'T', 9},
    {'J', 10},
    {'Q', 11},
    {'K', 12},
    {'A', 13}
};

static const std::vector<char> cardFromNumber = {
    '2', '3', '4', '5', '6', '7', '8', '9', 'T',
    'J', 'Q', 'K', 'A'
};

class Card
{
public:
    Card(char value, bool part2 = false) : value(value), number(card_values.at(value)) {
        if (part2 && value == 'J')
        {
            number = 0;
        }
    }

    bool operator<(const Card& other) const {
        return number < other.number;
    }

    bool operator==(const Card& other) const {
        return number == other.number;
    }

    bool operator!=(const Card& other) const {
        return number != other.number;
    }

    char getValue() const { return value; }

    int getNumber() const { return number; }

private:
    char value;
    int number;
};


class Hand
{
public:

    Hand(const std::string& hand, int f_bid, bool part2 = false) : bid(f_bid) {
        assert(hand.size() == 5);
        for (int i = 0; i < 5; ++i) {
            cards[i] = Card(hand[i], part2);
        }
        type = calculateType(part2);
    }

    bool operator<(const Hand& other) const {
        if (type != other.type) {
            return type < other.type;
        }

        for (int i = 0; i < 5; ++i) {
            if (cards[i] != other.cards[i]) {
                return cards[i] < other.cards[i];
            }
        }
        std::cout << "ERROR: Comparing two equal hands" << std::endl;
        return false;
    }

    bool operator==(const Hand& other) const {
        if (type != other.type) {
            return false;
        }

        for (int i = 0; i < 5; ++i) {
            if (cards[i] != other.cards[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Hand& other) const {
        return !(*this == other);
    }

    int getBid() const { return bid; }

    HandType getType() const { return type; }

private:
    std::array<Card, 5> cards {{'2', '2', '2', '2', '2'}};
    HandType type;
    int bid;

    HandType calculateType(bool part2) const {
        std::unordered_map<char, int> card_counts;
        for (const auto& card : cards) {
            ++card_counts[card.getValue()];
        }
        int numberOfJs = 0;
        if (part2 && card_counts.contains('J'))
        {
            if (card_counts.size() == 1)
            {
                return HandType::FIVE_OF_A_KIND;
            }
            numberOfJs = card_counts['J'];
            card_counts.erase('J');
        }
        std::vector<int> counts {};
        for (const auto& [_, count] : card_counts) {
            counts.push_back(count);
        }
        std::sort(counts.rbegin(), counts.rend());
        counts[0] += numberOfJs;

        if (counts[0] == 5)
            return HandType::FIVE_OF_A_KIND;
        if (counts[0] == 4)
            return HandType::FOUR_OF_A_KIND;
        if (counts[0] == 3 && counts[1] == 2)
            return HandType::FULL_HOUSE;
        if (counts[0] == 3)
            return HandType::THREE_OF_A_KIND;
        if (counts[0] == 2 && counts[1] == 2)
            return HandType::TWO_PAIR;
        if (counts[0] == 2)
            return HandType::ONE_PAIR;
        return HandType::HIGH_CARD;
    }
};


int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::vector<Hand> handsPart1;
    std::vector<Hand> handsPart2;
    std::string line;
    while (std::getline(file, line)) {
        auto splLine = split(line);
        Hand handPart1 {splLine[0], std::stoi(splLine[1]), false};
        Hand handPart2 {splLine[0], std::stoi(splLine[1]), true};
        auto it = std::upper_bound(handsPart1.begin(), handsPart1.end(), handPart1);
        handsPart1.insert(it, handPart1);
        auto it2 = std::upper_bound(handsPart2.begin(), handsPart2.end(), handPart2);
        handsPart2.insert(it2, handPart2);
    }
    int n = handsPart1.size();
    unsigned long part1 = 0;
    unsigned long part2 = 0;
    for (int i = 0; i < n; ++i) {
        part1 += handsPart1[i].getBid() * (i + 1);
        part2 += handsPart2[i].getBid() * (i + 1);
    }

    std::cout << "Part 1: " << part1 << '\n';
    std::cout << "Part 2: " << part2 << '\n';

    return 0;
}
