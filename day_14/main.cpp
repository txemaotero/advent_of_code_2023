#include <fstream>
#include <list>
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

    void tiltSouth() {
        for (uint32 i = nRows - 1; i < nRows; --i) {
            for (uint32 j = 0; j < nCols; ++j) {
                if (maze[i][j] != 2) {
                    continue;
                }
                uint32 destRow = i;
                while (destRow < nRows - 1 && maze[destRow + 1][j] == 0) {
                    ++destRow;
                }
                maze[i][j] = 0;
                maze[destRow][j] = 2;
            }
        }
    }

    void tiltWest() {
        for (uint32 i = 0; i < nRows; ++i) {
            for (uint32 j = 0; j < nCols; ++j) {
                if (maze[i][j] != 2) {
                    continue;
                }
                uint32 destCol = j;
                while (destCol > 0 && maze[i][destCol - 1] == 0) {
                    --destCol;
                }
                maze[i][j] = 0;
                maze[i][destCol] = 2;
            }
        }
    }

    void tiltEast () {
        for (uint32 i = 0; i < nRows; ++i) {
            for (uint32 j = nCols - 1; j < nCols; --j) {
                if (maze[i][j] != 2) {
                    continue;
                }
                uint32 destCol = j;
                while (destCol < nCols - 1 && maze[i][destCol + 1] == 0) {
                    ++destCol;
                }
                maze[i][j] = 0;
                maze[i][destCol] = 2;
            }
        }
    }

    void cycle() {
        tiltNorth();
        tiltWest();
        tiltSouth();
        tiltEast();
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

    int operator[](uint32 i, uint32 j) const {
        return maze[i][j];
    }

    bool operator==(const Maze& other) const {
        return maze == other.maze;
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
    Maze maze2 = maze;
    maze.tiltNorth();
    std::cout << "Part 1: " << maze.calcLoad() << "\n";

    std::list<Maze> mazeCache;
    uint64 i;
    for (i = 0; i < 1e9; ++i) {
        maze2.cycle();
        if (std::find(mazeCache.begin(), mazeCache.end(), maze2) != mazeCache.end()) {
            break;
        }
        if (mazeCache.size() < 100) {
            mazeCache.push_back(maze2);
        } else {
            mazeCache.pop_front();
            mazeCache.push_back(maze2);
        }
    }
    uint64 cycleEnd = i;
    uint64 cycleStart = i - (mazeCache.size() - std::distance(mazeCache.begin(), std::find(mazeCache.begin(), mazeCache.end(), maze2)));
    uint64 cycleLength = cycleEnd - cycleStart;
    uint64 remaining = static_cast<uint64>(1e9 - cycleStart) % cycleLength;
    for (i = 0; i < remaining - 1; ++i) {
        maze2.cycle();
    }

    std::cout << "Part 2: " << maze2.calcLoad() << "\n";

    return 0;
}
