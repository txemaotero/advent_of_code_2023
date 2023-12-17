#include <algorithm>
#include <fstream>
#include <ranges>
#include <numeric>
#include <string>
#include <vector>
#include <iostream>
#include "../utils.hpp"


struct Coord
{
    size_t row;
    size_t col;

    bool operator==(const Coord& other) const
    {
        return row == other.row && col == other.col;
    }

    bool operator!=(const Coord& other) const
    {
        return !(*this == other);
    }
};

std::ostream& operator<<(std::ostream& os, const Coord& coord)
{
    os << "(" << coord.row << ", " << coord.col << ")";
    return os;
}

void addSortedUnique(std::vector<size_t>& vec, size_t val)
{
    auto it = std::upper_bound(vec.begin(), vec.end(), val);
    if (it == vec.end()) {
        vec.push_back(val);
    } else if (*it != val) {
        vec.insert(it, val);
    }
}

std::vector<size_t> missingVals(const std::vector<size_t>& vec)
{
    std::vector<size_t> missing;
    if (vec.empty()) {
        return missing;
    }
    for (size_t i = 0; i < vec.size() - 1; ++i) {
        for (size_t j = vec[i] + 1; j < vec[i + 1]; ++j) {
            missing.push_back(j);
        }
    }
    return missing;
}

int numberOfMissingInBetween(const std::vector<size_t>& missingVals, size_t start, size_t end)
{
    if (start > end) {
        std::swap(start, end);
    }
    auto startIt = std::lower_bound(missingVals.begin(), missingVals.end(), start);
    auto endIt = std::upper_bound(missingVals.begin(), missingVals.end(), end);
    return std::distance(startIt, endIt);
}

size_t distance(const Coord& a, const Coord& b, const std::vector<size_t>& missingRows, const std::vector<size_t>& missingCols, unsigned int factor = 1)
{
    return std::abs(static_cast<int>(a.row) - static_cast<int>(b.row)) +
           std::abs(static_cast<int>(a.col) - static_cast<int>(b.col)) +
           factor * numberOfMissingInBetween(missingRows, a.row, b.row) +
           factor * numberOfMissingInBetween(missingCols, a.col, b.col);
}

int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    size_t row = 0;
    std::vector<Coord> galaxies;
    while (std::getline(file, line)) {
        for (size_t col{}; auto c: line) {
            if (c == '#') {
                galaxies.push_back({row, col});
            }
            ++col;
        }
        ++row;
    }
    std::vector<size_t> occupiedRows;
    std::vector<size_t> occupiedCols;
    for (auto galaxy: galaxies) {
        addSortedUnique(occupiedRows, galaxy.row);
        addSortedUnique(occupiedCols, galaxy.col);
    }
    auto missingRows = missingVals(occupiedRows);
    auto missingCols = missingVals(occupiedCols);

    size_t part1 = 0;
    size_t part2 = 0;
    for (int i{}; i < galaxies.size(); ++i) {
        for (int j{}; j < i; ++j) {
            part1 += distance(galaxies[i], galaxies[j], missingRows, missingCols);
            part2 += distance(galaxies[i], galaxies[j], missingRows, missingCols, 1000000u - 1);
        }
    }

    std::cout << "Part 1: " << part1 << '\n';
    std::cout << "Part 2: " << part2 << '\n';
    return 0;
}
