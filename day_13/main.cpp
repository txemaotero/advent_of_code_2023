#include <algorithm>
#include <fstream>
#include <optional>
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
        bool first = patternTransposed.empty();
        for (int i = 0; const auto& c : row) {
            newRow.push_back(c == '#');
            if (first) {
                patternTransposed.push_back({});
            }
            patternTransposed[i++].push_back(c == '#');
        }
        pattern.push_back(newRow);
    };

    void clear() {
        pattern.clear();
        patternTransposed.clear();
    }

    uint64 calcPart1(int diff = 0) {
        uint64 result = 0;
        auto reflexionColumn = calcReflexionColumn(diff);
        if (reflexionColumn)
        {
            result += (*reflexionColumn) + 1;
        }
        auto reflexionRow = calcReflexionRow(diff);
        if (reflexionRow)
        {
            result += 100*((*reflexionRow) + 1);
        }
        return result;
    }
    std::optional<size_t> calcReflexionColumn(int diff = 0)
    {
        return calcReflexionIndex(patternTransposed, diff);
    }

    std::optional<size_t> calcReflexionRow(int diff = 0)
    {
        return calcReflexionIndex(pattern, diff);
    }

private:
    std::vector<std::vector<bool>> pattern;
    std::vector<std::vector<bool>> patternTransposed;

    std::optional<size_t> calcReflexionIndex(const std::vector<std::vector<bool>>& matrix, int diff = 0)
    {
        for (size_t row = 0; row < matrix.size(); ++row) {
            if (checkReflexion(matrix, row, diff))
            {
                return row;
            }
        }
        return {};
    }

    bool checkReflexion(const std::vector<std::vector<bool>>& matrix, size_t row, int diff = 0)
    {
        size_t nReflexions = std::min(row+1, matrix.size() - row - 1);
        if (nReflexions == 0) {
            return false;
        }
        int differences = 0;
        for (size_t refIndex = 0; refIndex < nReflexions; ++refIndex) {
            for (size_t col = 0; col < matrix[row].size(); ++col) {
                if (matrix[row - refIndex][col] != matrix[row + refIndex + 1][col]) {
                    ++differences;
                    if (differences > diff) {
                        return false;
                    }
                }
            }
        }
        return differences == diff;
    }
};


int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    Pattern pattern;
    uint64 part1 = 0;
    uint64 part2 = 0;
    while (std::getline(file, line)) {
        if (line.empty()) {
            part1 += pattern.calcPart1();
            part2 += pattern.calcPart1(1);
            pattern.clear();
        }
        else
        {
            pattern.addRow(line);
        }
    }
    part1 += pattern.calcPart1();
    part2 += pattern.calcPart1(1);
    std::cout << "Part 1: " << part1 << "\n";
    std::cout << "Part 2: " << part2 << "\n";

    return 0;
}
