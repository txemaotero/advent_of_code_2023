#include <algorithm>
#include <fstream>
#include <ranges>
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
            return {0, -1};
        case Direction::DOWN:
            return {0, 1};
        case Direction::LEFT:
            return {-1, 0};
        case Direction::RIGHT:
            return {1, 0};
    }
}


struct Beam

{
    std::array<size_t, 2> pos;
    std::array<int, 2> dir;
};


class Maze
{
public:
    static constexpr uint32 N_ROWS = 110;
    static constexpr uint32 N_COLS = 136;

    void addRow(const std::string& row) {
        for (uint32 col_idx = 0; col_idx < N_COLS; ++col_idx) {
            maze[row_idx][col_idx] = row[col_idx];
        }
    }

    void addBeam(const Beam& beam) {
        beams.push_back(beam);
    }

    void moveBeamsOneStep() {
        std::vector<uint32> beams_to_remove;
        std::vector<Beam> new_beams;
        for (uint32 i = 0; auto& beam : beams) {
            addBeamPresence(beam);
            beam.pos[0] += beam.dir[0];
            beam.pos[1] += beam.dir[1];
            if (!contains(beam.pos))
                beams_to_remove.push_back(i);
            else
            {
                bool remove = processBeam(beam, new_beams);
                if (remove)
                    beams_to_remove.push_back(i);
            }
            ++i;
        }
    }

    bool contains(const std::array<size_t, 2>& pos) const {
        return pos[0] < N_ROWS && pos[1] < N_COLS;
    }

private:
    std::array<std::array<char, N_COLS>, N_ROWS> maze;
    std::array<std::array<std::vector<std::array<int, 2>>, N_COLS>, N_ROWS> beam_presence;
    std::vector<Beam> beams;
    uint32 row_idx = 0;

    void addBeamPresence(const Beam& beam) {
        if (!std::ranges::contains(beam_presence[beam.pos[0]][beam.pos[1]], beam.dir)) {
            beam_presence[beam.pos[0]][beam.pos[1]].push_back(beam.dir);
        }
    }

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
                    case Direction::UP:
                        beam.dir = toDir(Direction::RIGHT);
                    case Direction::DOWN:
                        beam.dir = toDir(Direction::LEFT);
                    case Direction::LEFT:
                        beam.dir = toDir(Direction::DOWN);
                    case Direction::RIGHT:
                        beam.dir = toDir(Direction::UP);
                }
            }
            case '\\':
            {
                switch (dir) {
                    case Direction::UP:
                        beam.dir = toDir(Direction::LEFT);
                    case Direction::DOWN:
                        beam.dir = toDir(Direction::RIGHT);
                    case Direction::LEFT:
                        beam.dir = toDir(Direction::UP);
                    case Direction::RIGHT:
                        beam.dir = toDir(Direction::DOWN);
                }
                return std::ranges::contains(beam_presence[beam.pos[0]][beam.pos[1]], beam.dir);
            }
            case '-':
            {
            }
            case '|':
            {
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

    return 0;
}
