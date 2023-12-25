#include <algorithm>
#include <fstream>
#include <ranges>
#include <string>
#include <iostream>
#include "../utils.hpp"


class Maze {
public:
    static constexpr uint32 nRows = 100;
    static constexpr uint32 nCols = 100;

    void addRow(const std::string& row) {
        for (uint32 i = 0; i < row.size(); ++i) {
            uint32 value;
            if (row[i] == '.') {
                value = 0;
            } else if (row[i] == '#') {
                value = 1;
            } else if (row[i] == 'O') {
                value = 2;
            } else {
                std::cerr << "Error: invalid character in maze\n";
                return;
            }
            maze[currentRow][i] = value;
        }
        ++currentRow;
    }

    void tiltNorth() {
        for (uint32 i = 0; i < nRows; ++i) {
            for (uint32 j = 0; j < nCols; ++j) {
                if (maze[i][j] != 2) {
                    continue;
                }
                uint32 destRow = i;
                while (destRow > 0 && maze[destRow - 1][j] == 0) {
                    --destRow;
                }
                maze[i][j] = 0;
                maze[destRow][j] = 2;
            }
        }
    }

    uint32 calcLoad() {
        uint32 load = 0;
        for (uint32 i = 0; i < nRows; ++i) {
            for (uint32 j = 0; j < nCols; ++j) {
                if (maze[i][j] == 2) {
                    load += (nRows - i);
                }
            }
        }
        return load;
    }

private:
    std::array<std::array<int, nCols>, nRows> maze;
    uint32 currentRow {0};
};


int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    Maze maze;
    while (std::getline(file, line)) {
        maze.addRow(line);
    }
    maze.tiltNorth();
    std::cout << "Part 1: " << maze.calcLoad() << "\n";

    return 0;
}
