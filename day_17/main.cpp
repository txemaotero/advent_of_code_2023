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

    bool operator==(const Situation& other)
    {
        return currentPos == other.currentPos && previousPos == other.previousPos && consecutiveStraightMoves == other.consecutiveStraightMoves;
    }

    bool isValid() const
    {
        return currentPos != previousPos;
    }

};


class PlayGround
{
public:
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

class IVisitedPositionManager
{
public:
    virtual void clear() = 0;
    virtual bool checkWasVisitedAndAdd(const Situation& situation) = 0;
    virtual ~IVisitedPositionManager() = default;
};

class VisitedPositionManager : public IVisitedPositionManager
{
public:
    VisitedPositionManager()
    {
        clear();
    }

    void clear() override
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

    bool checkWasVisitedAndAdd(const Situation& situation) override
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

class VisitedPositionManagerPart2: public VisitedPositionManager
{
public:
    void clear() override
    {
        for (auto& row : grid) {
            for (auto& element : row) {
                element.clear();
            }
        }
    }

    bool checkWasVisitedAndAdd(const Situation& situation) override
    {
        auto& element = getElement(situation);
        auto it = std::find(element.begin(), element.end(), situation);
        if (it == element.end()) {
            element.push_back(situation);
            return false;
        }
        if (it->accumulatedHeat <= situation.accumulatedHeat) {
            return true;
        }
        *it = situation;
        return false;
    }

private:

    std::array<std::array<std::vector<Situation>, PlayGround::COLS>, PlayGround::ROWS> grid {};

    std::vector<Situation>& getElement(const Situation& situation)
    {
        return grid[situation.currentPos.row][situation.currentPos.col];
    }

};

bool situationCmp(const Situation& a, const Situation& b)
{
    return a.heuristic() > b.heuristic();
}

class AStar
{
public:
    AStar(const PlayGround& playGround, const Position& start, const Position& end, bool _part1 = true) :
        playGround(playGround),
        start(start),
        end(end),
        part1(_part1)
    {
        if (part1)
        {
            visited = std::make_unique<VisitedPositionManager>();
        }
        else
        {
            visited = std::make_unique<VisitedPositionManagerPart2>();
        }
    }

    Situation findPath()
    {
        visited->clear();
        toCheck.clear();
        toCheck.push_back({start, start, 0, 0, manhatanDistance(start, end)});
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
                auto prevConsecutiveStraightMoves = situation.consecutiveStraightMoves;
                uint32 consecutiveStraightMoves;
                if (areAligned(situation.previousPos, situation.currentPos, neighbor))
                {
                    consecutiveStraightMoves = 1 + prevConsecutiveStraightMoves;
                }
                else
                {
                    consecutiveStraightMoves = 0;
                }

                if (part1)
                {
                    if (consecutiveStraightMoves > 2)
                        continue;
                }
                else
                {
                    if (situation.previousPos != situation.currentPos) // Special first case
                    {
                        if (consecutiveStraightMoves == 0 && prevConsecutiveStraightMoves < 3)
                            continue;
                        if (consecutiveStraightMoves > 9)
                            continue;
                    }
                }

                Situation newSituation {neighbor, situation.currentPos, consecutiveStraightMoves,
                    situation.accumulatedHeat + playGround[neighbor], manhatanDistance(neighbor, end)};

                if (visited->checkWasVisitedAndAdd(newSituation))
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
    bool part1;
    std::vector<Situation> toCheck; // Best candidates at the end
    std::unique_ptr<IVisitedPositionManager> visited;
};


int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    PlayGround playGround;
    while (std::getline(file, line)) {
        playGround.addRow(line);
    }
    AStar aStar(playGround, {0, 0}, {PlayGround::ROWS - 1, PlayGround::COLS - 1}, true);
    Situation endSituation = aStar.findPath();
    std::println("Part 1: {}", endSituation.accumulatedHeat);

    AStar aStar2(playGround, {0, 0}, {PlayGround::ROWS - 1, PlayGround::COLS - 1}, false);
     endSituation = aStar2.findPath();
    std::println("Part 2: {}", endSituation.accumulatedHeat);

    return 0;
}
