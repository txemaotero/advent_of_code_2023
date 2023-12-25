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

    uint64 calcPart1() {
        uint64 result = 0;
        auto reflexionColumn = calcReflexionColumn();
        if (reflexionColumn)
        {
            result += (*reflexionColumn) + 1;
        }
        auto reflexionRow = calcReflexionRow();
        if (reflexionRow)
        {
            result += 100*((*reflexionRow) + 1);
        }
        return result;
    }

    std::optional<size_t> calcReflexionColumn()
    {
        return calcReflexionIndex(patternTransposed);
    }

    std::optional<size_t> calcReflexionRow()
    {
        return calcReflexionIndex(pattern);
    }

private:
    std::vector<std::vector<bool>> pattern;
    std::vector<std::vector<bool>> patternTransposed;

    std::optional<size_t> calcReflexionIndex(const std::vector<std::vector<bool>>& matrix)
    {
        for (size_t row = 0; row < matrix.size(); ++row) {
            if (checkReflexion(matrix, row))
            {
                return row;
            }
        }
        return {};
    }

    bool checkReflexion(const std::vector<std::vector<bool>>& matrix, size_t row)
    {
        size_t nReflexions = std::min(row+1, matrix.size() - row - 1);
        if (nReflexions == 0) {
            return false;
        }
        for (size_t refIndex = 0; refIndex < nReflexions; ++refIndex) {
            if (!std::ranges::equal(matrix[row - refIndex], matrix[row + refIndex + 1], std::ranges::equal_to{})) {
                return false;
            }
        }
        return true;
    }
};


int main() {
    std::ifstream file("input.txt");
    // std::ifstream file("example.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    Pattern pattern;
    uint64 part1 = 0;
    int i = 1;
    while (std::getline(file, line)) {
        if (line.empty()) {
            auto aux = pattern.calcPart1();
            part1 += aux;
            pattern.clear();
        }
        else
        {
            pattern.addRow(line);
        }
    }
    auto aux = pattern.calcPart1();
    part1 += aux;
    std::cout << "Part 1: " << part1 << "\n";

    return 0;
}
