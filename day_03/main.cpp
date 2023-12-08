#include <algorithm>
#include <cctype>
#include <fstream>
#include <ranges>
#include <string>
#include <variant>
#include <vector>
#include <iostream>


struct Empty {};

struct Number
{
    int value;
    bool isValid;
};

class Array {
public:
    using Element = std::variant<Empty, Number, char>;
    Array() = default;

    void addRow(const std::string& line) {
        std::vector<Element> row;
        for (const auto& c : line) {
            if (c == '.')
            {
                row.push_back(Empty{});
            }
            else if (isdigit(c))
            {
                row.push_back(Number{std::stoi(std::string{c}), false});
            }
            else
            {
                row.push_back(c);
            }
        }
        data.push_back(row);
    }

    const std::vector<std::vector<Element>>& getData() const {
        return data;
    }

    auto numRows() const {
        return data.size();
    }

    auto numCols() const {
        return data[0].size();
    }

    void fillFullNums()
    {
        const auto& nRows = numRows();
        const auto& nCols = numCols();
        std::vector<int> fullNumsIdsRow {};
        fullNums.clear();
        fullNumIds.clear();
        int currentNumberId = 0;
        for (auto i = 0; i < nRows; ++i)
        {
            const auto& row = data[i];
            fullNumsIdsRow.clear();
            int currentNumber = 0;
            for (auto j = 0; j < nCols; ++j)
            {
                const auto& element = row[j];
                if (std::holds_alternative<Number>(element))
                {
                    const auto& number = std::get<Number>(element);
                    currentNumber = currentNumber * 10 + number.value;
                    fullNumsIdsRow.push_back(currentNumberId);
                }
                else
                {
                    if (currentNumber != 0)
                    {
                        fullNums.push_back(currentNumber);
                        ++currentNumberId;
                        currentNumber = 0;
                    }
                    fullNumsIdsRow.push_back(-1);
                }
            }
            if (currentNumber != 0)
            {
                fullNums.push_back(currentNumber);
                ++currentNumberId;
            }
            fullNumIds.push_back(fullNumsIdsRow);
        }
    }

    int sumValidNumbers()
    {
        if (fullNums.empty())
        {
            fillFullNums();
        }
        const auto& nRows = numRows();
        const auto& nCols = numCols();
        int result = 0;
        int lastAddedId = -1;
        for (auto i = 0; i < nRows; ++i)
        {
            const auto& row = data[i];
            for (auto j = 0; j < nCols; ++j)
            {
                const auto& element = row[j];
                if (std::holds_alternative<Number>(element))
                {
                    const auto& numId = fullNumIds[i][j];
                    if (lastAddedId != numId && isValidPosition(i, j, nRows, nCols))
                    {
                        result += fullNums[numId];
                        lastAddedId = numId;
                    }
                }
            }
        }
        return result;
    }

    int sumConnectedToGears()
    {
        if (fullNums.empty())
        {
            fillFullNums();
        }
        const auto& nRows = numRows();
        const auto& nCols = numCols();
        int result = 0;
        int lastAddedId = -1;
        for (auto i = 0; i < nRows; ++i)
        {
            const auto& row = data[i];
            for (auto j = 0; j < nCols; ++j)
            {
                const auto& element = row[j];
                if (std::holds_alternative<char>(element) && std::get<char>(element) == '*')
                {
                    auto connectedNums = getConnectedNums(i, j, nRows, nCols);
                    if (connectedNums.size() < 2)
                    {
                        continue;
                    }
                    else if (connectedNums.size() > 2)
                    {
                        std::cout << "Error: more than 2 connected numbers" << std::endl;
                        return -1;
                    }
                    result += connectedNums[0] * connectedNums[1];
                }
            }
        }
        return result;
    }

    std::vector<int> getConnectedNums(int r, int c, int nRows, int nCols)
    {
        std::vector<int> connectedIds;
        for (int toCheckRow = -1 ; toCheckRow <= 1; ++toCheckRow)
        {
            for (int toCheckCol = -1; toCheckCol <= 1; ++toCheckCol)
            {
                const auto& row = r + toCheckRow;
                const auto& col = c + toCheckCol;
                if (row >= 0 && row < nRows && col >= 0 && col < nCols)
                {
                    const auto& numId = fullNumIds[row][col];
                    if (numId != -1)
                    {
                        connectedIds.push_back(numId);
                    }
                }
            }
        }

        std::ranges::sort(connectedIds);
        auto last = std::ranges::unique(connectedIds);
        connectedIds.erase(last.begin(), connectedIds.end());

        std::vector<int> result;
        for (const auto& id : connectedIds)
        {
            result.push_back(fullNums[id]);
        }
        return result;
    }

private:
    std::vector<std::vector<Element>> data;
    std::vector<std::vector<int>> fullNumIds;
    std::vector<int> fullNums;

    bool isValidPosition(int r, int c, int nRows, int nCols)
    {
        bool isValid = false;
        for (int toCheckRow = -1 ; toCheckRow <= 1; ++toCheckRow)
        {
            for (int toCheckCol = -1; toCheckCol <= 1; ++toCheckCol)
            {
                const auto& row = r + toCheckRow;
                const auto& col = c + toCheckCol;
                if (row >= 0 && row < nRows && col >= 0 && col < nCols)
                {
                    const auto& element = data[row][col];
                    isValid = isValid || std::holds_alternative<char>(element);
                }
            }
        }
        return isValid;
    }
};


int main() {
    std::ifstream file("input.txt");

    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    Array data {};
    std::string line;
    while (std::getline(file, line)) {
        data.addRow(line);
    }

    std::cout << "Part 1: " << data.sumValidNumbers() << '\n';
    std::cout << "Part 2: " << data.sumConnectedToGears() << '\n';

    return 0;
}
