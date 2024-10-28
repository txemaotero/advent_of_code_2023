#include <algorithm>
#include <fstream>
#include <iostream>
#include <ostream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

enum class CellState
{
    empty,
    rock,
    occupied
};

CellState charToState(const char c)
{
    switch (c)
    {
        case '.':
            return CellState::empty;
        case '#':
            return CellState::rock;
        case 'S':
            return CellState::occupied;
        default:
            throw std::runtime_error("Unexpected character" + std::string{c});
    }
}

using GridRow = std::vector<CellState>;
using Grid = std::vector<GridRow>;
using Coord = std::array<size_t, 2>;

std::vector<CellState> getNeighbours(const Grid& grid, const Coord& coord, const Coord& dimensions)
{
    std::vector<CellState> result;
    if (coord[0] > 0)
        result.push_back(grid[coord[0] - 1][coord[1]]);
    if (coord[0] < (dimensions[0] - 1))
        result.push_back(grid[coord[0] + 1][coord[1]]);

    if (coord[1] > 0)
        result.push_back(grid[coord[0]][coord[1] - 1]);
    if (coord[1] < (dimensions[1] - 1))
        result.push_back(grid[coord[0]][coord[1] + 1]);

    return result;
}

Grid iterateOnce(const Grid& grid)
{
    Grid result{grid};
    const auto nrows = grid.size();
    const auto ncols = grid[0].size();
    for (size_t i = 0; i < nrows; ++i)
    {
        for (size_t j = 0; j < ncols; ++j)
        {
            const auto current = grid[i][j];
            auto& newValue = result[i][j];
            switch (current)
            {
                case CellState::empty:
                case CellState::occupied:
                {
                    const auto neig = getNeighbours(grid, {i, j}, {nrows, ncols});
                    newValue = std::ranges::find(neig, CellState::occupied) != neig.end() ?
                                   CellState::occupied :
                                   CellState::empty;
                    break;
                }
                case CellState::rock:
                    break;
            }
        }
    }
    return result;
}

size_t countSteps(const Grid& grid)
{
    return std::ranges::fold_left(grid | std::views::join,
                                  0,
                                  [](int count, const CellState state)
                                  {
                                      if (state == CellState::occupied)
                                          return count + 1;
                                      return count;
                                  });
}

int main()
{
    std::ifstream file("input.txt");
    if (!file)
    {
        std::cerr << "Error opening file\n";
        return 1;
    }

    Grid grid;
    std::string line;
    size_t nCols = 0;
    while (std::getline(file, line))
    {
        GridRow row;
        std::ranges::copy(std::views::transform(line, charToState), std::back_inserter(row));
        if (!nCols)
            nCols = row.size();
        else if (nCols != row.size())
            std::cout << "Unexpected n cols: " << nCols << ", " << row.size() << "\n";
        grid.emplace_back(std::move(row));
    }

    int iter = 64;
    while (iter--)
    {
        grid = iterateOnce(grid);
    }
    std::cout << "Part 1: " << countSteps(grid) << "\n";

    return 0;
}
