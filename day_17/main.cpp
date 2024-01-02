#include <algorithm>
#include <memory>
#include <optional>
#include <print>
#include <fstream>
#include <string>
#include <iostream>
#include "../utils.hpp"


enum class Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};


struct Position
{
    uint32 row;
    uint32 col;

    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
};


Direction getDirection(const Position& from, const Position& to) {
    if (from.row == to.row) {
        if (from.col < to.col) {
            return Direction::RIGHT;
        } else {
            return Direction::LEFT;
        }
    } else {
        if (from.row < to.row) {
            return Direction::DOWN;
        } else {
            return Direction::UP;
        }
    }
}

struct Node
{
    Position pos;
    uint32 g;
    uint32 h;
    uint32 f;
    uint32 consecutiveStraight;
    Direction direction;
    std::unique_ptr<Node> parent {};

    Node(const Position& pos, uint32 g, uint32 h, Direction direction, uint32 consecutiveStraight)
        : pos(pos)
        , g(g)
        , h(h)
        , f(g + h)
        , direction(direction)
        , consecutiveStraight(consecutiveStraight)
    {}

    Node(const Position& pos, uint32 g, uint32 h)
        : pos(pos)
        , g(g)
        , h(h)
        , f(g + h)
        , direction(Direction::NONE)
        , consecutiveStraight(0)
    {}

    bool operator<(const Node& other) const {
        return f < other.f;
    }

    bool operator==(const Node& other) const {
        return pos == other.pos && direction == other.direction && consecutiveStraight == other.consecutiveStraight;
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

bool areAligned(const Position& a, const Position& b, const Position& c)
{
    return (a.row == b.row && b.row == c.row) || (a.col == b.col && b.col == c.col);
}


class AStar
{
public:
    AStar(const PlayGround& playGround, const Position& start, const Position& end)
        : playGround(playGround)
        , start(start)
        , end(end)
    {
        auto startNode = std::make_unique<Node>(start, 0, manhatanDistance(start, end));
        toVisit.reserve(PlayGround::ROWS * PlayGround::COLS);
        visited.reserve(PlayGround::ROWS * PlayGround::COLS);
        toVisit.push_back(std::move(startNode));
    }

    std::unique_ptr<Node> findPath() {
        while (!toVisit.empty()) {
            auto current = std::move(toVisit.back());
            toVisit.pop_back();

            if (current->pos == end) {
                return current;
            }
            auto& parent = current->parent;

            for (const auto& neighbor : getNeighbors(current->pos)) {

                if (parent && neighbor == parent->pos) {
                    continue;
                }

                uint32 e = playGround[neighbor.row, neighbor.col];
                uint32 g = current->g + playGround[neighbor.row, neighbor.col];
                uint32 h = manhatanDistance(neighbor, end);
                uint32 f = g + h;

                auto newNode = std::make_unique<Node>(neighbor, g, h);

                uint32 consecutiveStraight = 1;
                if (parent && areAligned(parent->pos, current->pos, neighbor)) {
                    consecutiveStraight = current->consecutiveStraight + 1;
                }
                newNode->consecutiveStraight = consecutiveStraight;
                newNode->direction = getDirection(current->pos, neighbor);

                if (newNode->consecutiveStraight > 3) {
                    continue;
                }
                auto it = std::ranges::find(toVisit, newNode);
                if (it != toVisit.end())
                {
                }
                else
                {
                }
            }
        }
        return {};
    }

private:
    std::vector<std::unique_ptr<Node>> toVisit;
    std::vector<std::unique_ptr<Node>> visited;
    Position start;
    Position end;
    const PlayGround& playGround;
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
    AStar aStar(playGround, {0, 0}, {PlayGround::ROWS - 1, PlayGround::COLS - 1});
    auto node = aStar.findPath();
    std::cout << "Path length: " << node.g << '\n';
    // while (node.parent != nullptr) {
    //     std::cout << node.pos.row << ", " << node.pos.col << '\n';
    //     node = *(node.parent);
    // }

    return 0;
}
