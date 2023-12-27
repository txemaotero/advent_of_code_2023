#include <algorithm>
#include <fstream>
#include <print>
#include <format>
#include <ostream>
#include <string>
#include <iostream>
#include "../utils.hpp"


enum class Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

Direction getDirection(const std::array<int, 2>& dir) {
    if (dir[0] == 0) {
        if (dir[1] == 1)
            return Direction::RIGHT;
        else
            return Direction::LEFT;
    } else {
        if (dir[0] == 1)
            return Direction::DOWN;
        else
            return Direction::UP;
    }
}

std::array<int, 2> toDir(const Direction& dir) {
    switch (dir) {
        case Direction::UP:
            return {-1, 0};
        case Direction::DOWN:
            return {1, 0};
        case Direction::LEFT:
            return {0, -1};
        case Direction::RIGHT:
            return {0, 1};
    }
}


struct Beam

{
    std::array<int, 2> pos;
    std::array<int, 2> dir;

    std::string toString() const {
        return std::format("Beam(pos: ({}, {}), dir: ({}, {}))", pos[0], pos[1], dir[0], dir[1]);
    }
};


class Maze
{
public:
    static constexpr uint32 N_ROWS = 110;
    static constexpr uint32 N_COLS = 110;
    // static constexpr uint32 N_ROWS = 10;
    // static constexpr uint32 N_COLS = 10;

    void addRow(const std::string& row) {
        for (uint32 col_idx = 0; col_idx < N_COLS; ++col_idx) {
            maze[row_idx][col_idx] = row[col_idx];
        }
        ++row_idx;
    }

    void addBeam(const Beam& beam) {
        Beam new_beam = beam;
        bool hasToBeRemoved = processBeam(new_beam, beams);
        if (!hasToBeRemoved) {
            beams.push_back(new_beam);
        }
    }

    void moveBeamsOneStep() {
        std::vector<uint32> beams_to_remove;
        std::vector<Beam> new_beams;
        for (uint32 i = 0; auto& beam : beams) {
            if (addBeamPresence(beam))
            {
                beams_to_remove.push_back(i);
                continue;
            }
            beam.pos[0] += beam.dir[0];
            beam.pos[1] += beam.dir[1];
            if (!insideMaze(beam.pos))
            {
                beams_to_remove.push_back(i);
            }
            else if (processBeam(beam, new_beams))
            {
                beams_to_remove.push_back(i);
            }
            ++i;
        }
        for (auto i : std::views::reverse(beams_to_remove)) {
            beams.erase(beams.begin() + i);
        }
        for (auto& beam : new_beams) {
            beams.push_back(beam);
        }
    }

    void simulateBeams() {
        while (!beams.empty()) {
            moveBeamsOneStep();
        }
    }

    void clear() {
        beams.clear();
        for (auto& row : beam_presence) {
            for (auto& tile : row) {
                tile.clear();
            }
        }
    }

    bool insideMaze(const std::array<int, 2>& pos) const {
        return pos[0] < N_ROWS && pos[1] < N_COLS && pos[0] >= 0 && pos[1] >= 0;
    }

    uint32 getEnergizedTiles() const {
        uint32 count = 0;
        for (uint32 row_idx = 0; row_idx < N_ROWS; ++row_idx) {
            for (uint32 col_idx = 0; col_idx < N_COLS; ++col_idx) {
                if (beam_presence[row_idx][col_idx].size() > 0)
                    ++count;
            }
        }
        return count;
    }

    const auto& getMaze() const {
        return maze;
    }

    void printState() const {
        for (uint32 row_idx = 0; row_idx < N_ROWS; ++row_idx) {
            for (uint32 col_idx = 0; col_idx < N_COLS; ++col_idx) {
                if (beam_presence[row_idx][col_idx].size() > 0)
                    std::print("#");
                else
                    std::print(".");
            }
            std::println("");
        }
    }

private:
    std::array<std::array<char, N_COLS>, N_ROWS> maze;
    std::array<std::array<std::vector<std::array<int, 2>>, N_COLS>, N_ROWS> beam_presence;
    std::vector<Beam> beams;
    uint32 row_idx = 0;

    /**
     * @brief Returns true if the beam was already processed
     */
    bool addBeamPresence(const Beam& beam) {
        if (!std::ranges::contains(beam_presence[beam.pos[0]][beam.pos[1]], beam.dir)) {
            beam_presence[beam.pos[0]][beam.pos[1]].push_back(beam.dir);
            return false;
        }
        return true;
    }

    /**
     * @brief Returns true if the resulting beam must be removed
     */
    bool processBeam(Beam& beam, std::vector<Beam>& new_beams) {
        Direction dir = getDirection(beam.dir);
        char c = maze[beam.pos[0]][beam.pos[1]];
        switch (c) {
            case '.':
            {
                return std::ranges::contains(beam_presence[beam.pos[0]][beam.pos[1]], beam.dir);
            }
            case '/':
            {
                switch (dir) {
                    case Direction::UP: {
                        beam.dir = toDir(Direction::RIGHT);
                        break;
                    }
                    case Direction::DOWN: {
                        beam.dir = toDir(Direction::LEFT);
                        break;
                    }
                    case Direction::LEFT: {
                        beam.dir = toDir(Direction::DOWN);
                        break;
                    }
                    case Direction::RIGHT: {
                        beam.dir = toDir(Direction::UP);
                        break;
                    }
                }
                return std::ranges::contains(beam_presence[beam.pos[0]][beam.pos[1]], beam.dir);
            }
            case '\\':
            {
                switch (dir) {
                    case Direction::UP: {
                        beam.dir = toDir(Direction::LEFT);
                        break;
                    }
                    case Direction::DOWN: {
                        beam.dir = toDir(Direction::RIGHT);
                        break;
                    }
                    case Direction::LEFT: {
                        beam.dir = toDir(Direction::UP);
                        break;
                    }
                    case Direction::RIGHT: {
                        beam.dir = toDir(Direction::DOWN);
                        break;
                    }
                }
                return std::ranges::contains(beam_presence[beam.pos[0]][beam.pos[1]], beam.dir);
            }
            case '-':
            {
                switch (dir) {
                    case Direction::UP:
                    case Direction::DOWN:
                    {
                        beam.dir = toDir(Direction::LEFT);
                        Beam new_beam {beam.pos, toDir(Direction::RIGHT)};
                        if (!std::ranges::contains(beam_presence[beam.pos[0]][beam.pos[1]], new_beam.dir)) {
                            new_beams.push_back(new_beam);
                        }
                        return std::ranges::contains(beam_presence[beam.pos[0]][beam.pos[1]], beam.dir);
                    }
                    case Direction::LEFT:
                    case Direction::RIGHT:
                    {
                        return std::ranges::contains(beam_presence[beam.pos[0]][beam.pos[1]], beam.dir);
                    }
                }
            }
            case '|':
            {
                switch (dir) {
                    case Direction::UP:
                    case Direction::DOWN:
                    {
                        return std::ranges::contains(beam_presence[beam.pos[0]][beam.pos[1]], beam.dir);
                    }
                    case Direction::LEFT:
                    case Direction::RIGHT:
                    {
                        beam.dir = toDir(Direction::UP);
                        Beam new_beam {beam.pos, toDir(Direction::DOWN)};
                        if (!std::ranges::contains(beam_presence[beam.pos[0]][beam.pos[1]], new_beam.dir)) {
                            new_beams.push_back(new_beam);
                        }
                        return std::ranges::contains(beam_presence[beam.pos[0]][beam.pos[1]], beam.dir);
                    }
                }
            }
        }
        return false;
    }
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
    maze.addBeam({{0, 0}, toDir(Direction::RIGHT)});
    maze.simulateBeams();
    std::println("Part 1: {}", maze.getEnergizedTiles());

    maze.clear();
    uint32 part2 = 0;
    for (int i = 0; i < Maze::N_ROWS; ++i) {
        maze.addBeam({{i, 0}, toDir(Direction::RIGHT)});
        maze.simulateBeams();
        part2 = std::max(part2, maze.getEnergizedTiles());
        maze.clear();
        maze.addBeam({{i, Maze::N_ROWS - 1}, toDir(Direction::LEFT)});
        maze.simulateBeams();
        part2 = std::max(part2, maze.getEnergizedTiles());
        maze.clear();
    }
    for (int i = 0; i < Maze::N_COLS; ++i) {
        maze.addBeam({{0, i}, toDir(Direction::DOWN)});
        maze.simulateBeams();
        part2 = std::max(part2, maze.getEnergizedTiles());
        maze.clear();
        maze.addBeam({{Maze::N_COLS - 1, i}, toDir(Direction::UP)});
        maze.simulateBeams();
        part2 = std::max(part2, maze.getEnergizedTiles());
        maze.clear();
    }
    std::println("Part 2: {}", part2);
    return 0;
}

