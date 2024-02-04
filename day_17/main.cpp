#include <algorithm>
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
    // static constexpr uint32 ROWS = 13;
    // static constexpr uint32 COLS = 13;
    static constexpr uint32 ROWS = 141;
    static constexpr uint32 COLS = 141;

    void addRow(const std::string& row) {
        for (uint32 i = 0; i < COLS; ++i) {
            auto aux = std::stoi(row.substr(i, 1));
            grid[rowIdx][i] = aux;
        }
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

bool ordered(const std::vector<Situation>& toCheck)
{
    return std::is_sorted(toCheck.begin(), toCheck.end(), [](const Situation& a, const Situation& b) {
        return a.heuristic() > b.heuristic();
    });
}

class VisitedPositionManager
{
public:
    VisitedPositionManager()
    {
        for (auto& row : grid) {
            for (auto& element : row) {
                for (auto& dir : element) {
                    for (auto& consecutive : dir) {
                        consecutive = std::numeric_limits<unsigned int>::max();
                    }
                }
            }
        }
    }

    bool checkAndAdd(const Situation& situation)
    {
        auto& element = getElement(situation);
        if (element <= situation.accumulatedHeat) {
            return true;
        }
        element = situation.accumulatedHeat;
        return false;
    }

private:

    typedef std::array<std::array<unsigned int, 3>, 4> PossibleSituationsInPosition;
    std::array<std::array<PossibleSituationsInPosition, PlayGround::COLS>, PlayGround::ROWS> grid {};

    size_t getPrevPosIndex(const Situation& situation) const
    {
        if (situation.currentPos.col == situation.previousPos.col)
        {
            return situation.currentPos.row < situation.previousPos.row;
        }
        return 2 + (situation.currentPos.col < situation.previousPos.col);
    }

    unsigned int& getElement(const Situation& situation)
    {
        size_t prevPosIndex = getPrevPosIndex(situation);
        return grid[situation.currentPos.row][situation.currentPos.col][prevPosIndex][situation.consecutiveStraightMoves];
    }

};

bool situationCmp(const Situation& a, const Situation& b)
{
    return a.heuristic() > b.heuristic();
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
                // It can not turn back
                if (neighbor == situation.previousPos)
                {
                    continue;
                }
                uint32 consecutiveStraightMoves;
                if (areAligned(situation.previousPos, situation.currentPos, neighbor))
                {
                    consecutiveStraightMoves = 1 + situation.consecutiveStraightMoves;
                    if (consecutiveStraightMoves > 2)
                    {
                        continue;
                    }
                }
                else
                {
                    consecutiveStraightMoves = 0;
                }

                Situation newSituation {neighbor, situation.currentPos, consecutiveStraightMoves,
                    situation.accumulatedHeat + playGround[neighbor], manhatanDistance(neighbor, end)};

                if (visited->checkAndAdd(newSituation))
                {
                    continue;
                }

                auto it = std::lower_bound(toCheck.begin(), toCheck.end(), newSituation, situationCmp);
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
    std::unique_ptr<VisitedPositionManager> visited {std::make_unique<VisitedPositionManager>()};
};


int main() {
    std::ifstream file("input.txt");
    // std::ifstream file("example.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    PlayGround playGround;
    while (std::getline(file, line)) {
        playGround.addRow(line);
    }
    AStar aStar(playGround, {0, 0}, {PlayGround::ROWS - 1, PlayGround::COLS - 1});
    std::println("Finding the path...");
    Situation endSituation = aStar.findPath();
    if (!endSituation.isValid())
    {
        std::println("Invalid output. Path not found");
        return 0;
    }
    std::println("Part 1: {}", endSituation.accumulatedHeat);

    return 0;
}
