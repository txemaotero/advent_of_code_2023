#include <algorithm>
#include <fstream>
#include <string>
#include <iostream>
#include "../utils.hpp"


struct SolutionLine
{
    int part1;
    int part2;

    SolutionLine& operator+=(const SolutionLine& other) {
        part1 += other.part1;
        part2 += other.part2;
        return *this;
    }
};


int calcPart2(const std::vector<int>& firstValues) {
    int result = 0;
    for (const auto& val : firstValues | std::views::reverse)
    {
        result = val - result;
    }
    return result;
}

SolutionLine evalLine(std::vector<int>& vals) {
    SolutionLine result {};
    std::vector<int> leftVals;
    while (vals.size() > 0 && !std::all_of(vals.begin(), vals.end(), [](int x) { return x == 0; })) {
        result.part1 += vals.back();
        leftVals.push_back(vals.front());
        for (int i{}; auto [left, right] : vals | std::views::adjacent<2>) {
            vals[i++] = right - left;
        }
        vals.pop_back();
    }
    result.part2 = calcPart2(leftVals);
    return result;
}


int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    SolutionLine result {};
    std::string line;
    while (std::getline(file, line)) {
        auto valsStr = split(line);
        std::vector<int> vals;
        for (auto& valStr : valsStr) {
            vals.push_back(std::stoi(valStr));
        }
        result += evalLine(vals);
    }

    std::cout << "Part 1: " << result.part1 << "\n";
    std::cout << "Part 2: " << result.part2 << "\n";
    return 0;
}
