#include <cassert>
#include <fstream>
#include <optional>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include "../utils.hpp"


struct Coord
{
    size_t row;
    size_t col;

    bool operator==(const Coord& other) const
    {
        return row == other.row && col == other.col;
    }

    bool operator!=(const Coord& other) const
    {
        return !(*this == other);
    }
};

struct PipesArray
{
    using row_t = std::vector<char>;
    using matrix_t = std::vector<row_t>;
    using coord_t = Coord;

    matrix_t matrix;

    size_t rows() const { return matrix.size(); }
    size_t cols() const { return matrix[0].size(); }

    char& operator[](const coord_t& coord) { return matrix[coord.row][coord.col]; }
    const char& operator[](const coord_t& coord) const { return matrix[coord.row][coord.col]; }

    std::array<coord_t, 2> get_neighbours(coord_t coord) const
    {
        switch ((*this)[coord]) {
            case '|':
                return { { { coord.row - 1, coord.col }, { coord.row + 1, coord.col } } };
            case '-':
                return { { { coord.row, coord.col - 1 }, { coord.row, coord.col + 1 } } };
            case 'L':
                return { { { coord.row - 1, coord.col }, { coord.row, coord.col + 1 } } };
            case 'J':
                return { { { coord.row - 1, coord.col }, { coord.row, coord.col - 1 } } };
            case '7':
                return { { { coord.row + 1, coord.col }, { coord.row, coord.col - 1 } } };
            case 'F':
                return { { { coord.row + 1, coord.col }, { coord.row, coord.col + 1 } } };
            default:
                {
                    assert(false);
                    return { { { 0, 0 }, { 0, 0 } } };
                }
        }
    }
};


bool is_corner(char c)
{
    return c == 'L' || c == 'J' || c == '7' || c == 'F';
}


int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    PipesArray pipes {};
    Coord start_coord { 0, 0 };
    while (std::getline(file, line)) {
        std::vector<char> vec;
        for (auto c : line) {
            if (c == 'S')
            {
                c = '7'; // Inspecting the file
                start_coord = { pipes.matrix.size(), vec.size() };
            }
            vec.push_back(c);
        }
        pipes.matrix.push_back(vec);
    }

    int pipe_length = 0;
    int cummulArea = 0;
    std::optional<Coord> currentCorner = std::nullopt;
    std::optional<Coord> lastCorner = std::nullopt;
    Coord previous = start_coord;
    Coord current = pipes.get_neighbours(start_coord)[0];
    while (current != start_coord)
    {
        if (is_corner(pipes[current]))
        {
            if (!lastCorner.has_value())
            {
                lastCorner = current;
            }
            else if (!currentCorner.has_value())
            {
                currentCorner = current;
            }
            else
            {
                cummulArea += (currentCorner->row * lastCorner->col - lastCorner->row * currentCorner->col);
                lastCorner = currentCorner;
                currentCorner = current;
            }
        }
        auto neighbours = pipes.get_neighbours(current);
        if (neighbours[0] == previous)
        {
            previous = current;
            current = neighbours[1];
        }
        else
        {
            previous = current;
            current = neighbours[0];
        }
        ++pipe_length;
    }
    if (currentCorner.has_value())
    {
        cummulArea += (currentCorner->row * lastCorner->col - lastCorner->row * currentCorner->col);
    }
    cummulArea = (std::abs(cummulArea) - pipe_length ) / 2;
    int part1 = pipe_length / 2 + pipe_length % 2;
    std::cout << "Part 1: " << part1 << '\n';
    std::cout << "Part 2: " << cummulArea << '\n';
    return 0;
}
