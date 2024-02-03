#include <algorithm>
#include <memory>
#include <print>
#include <fstream>
#include <string>
#include <iostream>
#include "../utils.hpp"


struct Position
{
    uint32 row;
    uint32 col;

    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
};

bool areAligned(const Position& a, const Position& b, const Position& c)
{
    if (a == b || b == c)
        return false;
    return (a.row == b.row && b.row == c.row) || (a.col == b.col && b.col == c.col);
}


/**
 * @brief A situation while finding the path
 */
struct Situation
{
    Position currentPos;
    Position previousPos;
    uint32 consecutiveStraightMoves;
    uint32 accumulatedHeat;
    uint32 distanceToEnd;

    uint32 heuristic() const {
        return accumulatedHeat + distanceToEnd;
    }

    bool isValidNeighbor(const Position& neighbor) const {
        if (consecutiveStraightMoves < 3)
            return true;
        return !areAligned(previousPos, currentPos, neighbor);
    }

    bool operator<(const Situation& other)
    {
        return this->heuristic() < other.heuristic();
    }

    bool isValid() const
    {
        return currentPos != previousPos;
    }

};


class PlayGround
{
public:
    static constexpr uint32 ROWS = 13;
    static constexpr uint32 COLS = 13;
    // static constexpr uint32 ROWS = 141;
    // static constexpr uint32 COLS = 141;

    void addRow(const std::string& row) {
        for (uint32 i = 0; i < COLS; ++i) {
            auto aux = std::stoi(row.substr(i, 1));
            std::cout << aux;
            grid[rowIdx][i] = aux;
        }
        std::cout << std::endl;
        ++rowIdx;
    }

    uint32 operator[](uint32 row, uint32 col) const {
        return grid[row][col];
    }

    uint32 operator[](const Position& pos) const {
        return grid[pos.row][pos.col];
    }

private:
    uint32 rowIdx = 0;
    std::array<std::array<uint32, COLS>, ROWS> grid;
};


std::vector<Position> getNeighbors(const Position& pos)
{
    std::vector<Position> neighbors;
    if (pos.row > 0) {
        neighbors.push_back({pos.row - 1, pos.col});
    }
    if (pos.row < PlayGround::ROWS - 1) {
        neighbors.push_back({pos.row + 1, pos.col});
    }
    if (pos.col > 0) {
        neighbors.push_back({pos.row, pos.col - 1});
    }
    if (pos.col < PlayGround::COLS - 1) {
        neighbors.push_back({pos.row, pos.col + 1});
    }
    return neighbors;
}

uint32 manhatanDistance(const Position& a, const Position& b)
{
    return std::abs(static_cast<int32>(a.row) - static_cast<int32>(b.row)) +
           std::abs(static_cast<int32>(a.col) - static_cast<int32>(b.col));
}

class AStar
{
public:
    AStar(const PlayGround& playGround, const Position& start, const Position& end) :
        playGround(playGround),
        start(start),
        end(end)
    {
        toCheck.push_back({start, start, 0, 0, manhatanDistance(start, end)});
    }

    Situation findPath()
    {
        while (!toCheck.empty())
        {
            const Situation situation = toCheck.back();
            toCheck.pop_back();
            if (situation.currentPos == end)
            {
                return situation;
            }
            for (const Position& neighbor : getNeighbors(situation.currentPos))
            {
                if (!situation.isValidNeighbor(neighbor))
                {
                    continue;
                }
                uint32 consecutiveStraightMoves;
                if (areAligned(situation.previousPos, situation.currentPos, neighbor))
                {
                    consecutiveStraightMoves = 1 + situation.consecutiveStraightMoves;
                }
                else
                {
                    consecutiveStraightMoves = 0;
                }

                Situation newSituation {neighbor, situation.currentPos, consecutiveStraightMoves,
                    situation.accumulatedHeat + playGround[neighbor], manhatanDistance(neighbor, end)};
                auto it = std::lower_bound(toCheck.begin(), toCheck.end(), newSituation, [](const Situation& a, const Situation& b) {
                        return a.heuristic() < b.heuristic();
                        });
                toCheck.insert(it, newSituation);
            }
        }
        return {start, start, 0, 0, 0};
    }

private:
    const PlayGround& playGround;
    const Position start;
    const Position end;
    // Best candidates at the end
    std::vector<Situation> toCheck;
};


int main() {
    // std::ifstream file("input.txt");
    std::ifstream file("example.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    PlayGround playGround;
    while (std::getline(file, line)) {
        playGround.addRow(line);
    }
    std::println("Playground loaded");
    AStar aStar(playGround, {0, 0}, {PlayGround::ROWS - 1, PlayGround::COLS - 1});
    std::println("Finding the path...");
    Situation endSituation = aStar.findPath();
    if (!endSituation.isValid())
    {
        std::println("Invalid output. Path not found");
        return 0;
    }
    std::println("Path found, heat accumulated = %i", endSituation.accumulatedHeat);

    return 0;
}
