#include <fstream>
#include <optional>
#include <ranges>
#include <string>
#include <iostream>
#include "../utils.hpp"


class Pattern {
public:

    size_t nRows() const {
        return pattern.size();
    }

    size_t nCols() const {
        return pattern[0].size();
    }

    bool operator[](size_t i, size_t j) const {
        return pattern[i][j];
    };

    void addRow(const std::string& row) {
        std::vector<bool> newRow;
        for (const auto& c : row) {
            newRow.push_back(c == '#');
        }
        pattern.push_back(newRow);
    };

    void clear() {
        pattern.clear();
    }

    uint64 calcPart1() {
        uint64 result = 0;
        auto reflexionColumn = calcReflexionColumn();
        if (reflexionColumn)
            result += (*reflexionColumn) + 1;
        auto reflexionRow = calcReflexionRow();
        if (reflexionRow)
            result += 100*((*reflexionRow) + 1);
        return result;
    }

    std::optional<size_t> calcReflexionColumn()
    {
        return {};
    }

    std::optional<size_t> calcReflexionRow()
    {
        return {};
    }

private:
    std::vector<std::vector<bool>> pattern;
};


int main() {
    // std::ifstream file("input.txt");
    std::ifstream file("example.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    Pattern pattern;
    uint64 part1 = 0;
    while (std::getline(file, line)) {
        if (line.empty()) {
            part1 += pattern.calcPart1();
            pattern.clear();
        }
        else
        {
            pattern.addRow(line);
        }
    }
    part1 += pattern.calcPart1();
    std::cout << "Part 1: " << part1 << "\n";

    return 0;
}
