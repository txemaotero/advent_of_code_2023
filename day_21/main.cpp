#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <ostream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>
#include "../utils.hpp"

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

char stateToChar(CellState state)
{
    switch (state)
    {
        case CellState::empty:
            return '.';
        case CellState::rock:
            return '#';
        case CellState::occupied:
            return 'S';
        default:
            throw std::runtime_error("Invalid CellState");
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

Grid iterate(Grid grid, size_t ntimes)
{
    if (ntimes == 0)
        return grid;
    while (ntimes--)
        grid = iterateOnce(grid);

    return grid;
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

Grid getSubGrid(const Grid& grid,
                const size_t minX,
                const size_t maxX,
                const size_t minY,
                const size_t maxY)
{
    Grid result;
    const size_t nX = maxX - minX;
    result.reserve(nX);
    for (const auto& row : std::views::counted(grid.begin() + minX, nX))
    {
        result.emplace_back(row.begin() + minY, row.begin() + maxY);
    }
    return result;
}

void printGrid(const Grid& grid)
{
    std::ranges::copy(grid |
                          std::views::transform(
                              [](const GridRow& row)
                              {
                                  return row | std::views::transform(stateToChar);
                              }) |
                          std::views::join_with('\n'),
                      std::ostream_iterator<char>(std::cout));
    std::cout << '\n'; // Optional: Add a final newline if needed
}


Grid replicate(const Grid& grid, const size_t nx, const size_t ny)
{
    auto extended_rows =
        grid | std::views::transform(
                   [ny](const GridRow& row) -> GridRow
                   {
                       GridRow result;
                       result.reserve(row.size() * ny);
                       std::ranges::copy(std::views::repeat(row, ny) | std::views::join,
                                         std::back_inserter(result));
                       return result;
                   });

    std::vector<GridRow> extended_rows_vector(extended_rows.begin(), extended_rows.end());

    Grid result;
    result.reserve(grid.size() * nx);
    std::ranges::copy(std::views::repeat(extended_rows_vector, nx)
        | std::views::join, std::back_inserter(result));

    return result;
}

Grid replicateExample(Grid grid, size_t iter)
{
    if (iter == 0)
        return grid;
    size_t mid = grid.size() / 2;
    std::cout << mid << "\n";
    grid[mid][mid] = CellState::empty;

    size_t n = iter*2 + 1;
    grid = replicate(grid, n, n);

    mid = grid.size() / 2;
    std::cout << mid << "\n";
    grid[mid][mid] = CellState::occupied;
    return grid;
}

std::vector<std::vector<size_t>> countSubGrids(const Grid& grid)
{
    constexpr size_t SUB_SIZE = 131;
    const size_t nSubs = grid.size() / SUB_SIZE;
    if (grid.size() % SUB_SIZE != 0)
    {
        std::puts("Bad number");
        return {};
    }
    std::vector<std::vector<size_t>> result;
    for (size_t i = 0; i < nSubs; ++i)
    {
        std::vector<size_t> aux;
        for (size_t j = 0; j < nSubs; ++j)
        {
            auto sub = getSubGrid(grid,
                                  i * SUB_SIZE,
                                  (i + 1) * SUB_SIZE,
                                  j * SUB_SIZE,
                                  (j + 1) * SUB_SIZE);
            aux.push_back(countSteps(sub));
        }
        result.emplace_back(std::move(aux));
    }
    return result;
}

void printGridSubCounts(const Grid& grid)
{
    const auto subCounts = countSubGrids(grid);
    for (const auto& row: subCounts)
    {
        std::cout << "[";
        std::cout << join(row, ", ");
        std::cout << "]\n";
    }
}

// The grid is 131x131. S is placed in the middle (possition 65 (0-based)). The row and column where S is are empty.

int main()
{
    std::ifstream file("input.txt");
    if (!file)
    {
        std::cerr << "Error opening file\n";
        return 1;
    }

    Grid original;
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
        original.emplace_back(std::move(row));
    }

    std::cout << "Part 1: " << countSteps(iterate(original, 64)) << "\n";

    // Part 2: How many in 26501365 steps?
    // Notice that: 26501365 = 202300 * 131 + 65

    const size_t N = (26501365 - 65) / 131;

    // Lets see what we get for a lower N (=4)
    const size_t replica = 4;
    Grid grid = replicateExample(original, replica);

    grid = iterate(grid, 65);
    for (size_t i = 1; i <= replica; ++i)
    {
        grid = iterate(grid, 131);
    }
    printGridSubCounts(grid);
    // This is the output:
    // [0,    0,    0,    934,  5447, 900,  0,    0,    0]
    // [0,    0,    934,  6352, 7257, 6321, 900,  0,    0]
    // [0,    934,  6352, 7257, 7226, 7257, 6321, 900,  0]
    // [934,  6352, 7257, 7226, 7257, 7226, 7257, 6321, 900]
    // [5486, 7257, 7226, 7257, 7226, 7257, 7226, 7257, 5422]
    // [916,  6360, 7257, 7226, 7257, 7226, 7257, 6327, 945]
    // [0,    916,  6360, 7257, 7226, 7257, 6327, 945,  0]
    // [0,    0,    916,  6360, 7257, 6327, 945,  0,    0]
    // [0,    0,    0,    916,  5461, 945,  0,    0,    0]


    // Scale it to N
    const size_t sol = 5422 * 1 + 5447 * 1 + 5461 * 1 + 5486 * 1 + 6321 * (N - 1) + 6327 * (N - 1) +
                       6352 * (N - 1) + 6360 * (N - 1) + 7226 * (N - 1) * (N - 1) + 7257 * N * N +
                       900 * N + 916 * N + 934 * N + 945 * N;

    std::cout << std::format("Part 2: {}\n", sol);

    return 0;
}
