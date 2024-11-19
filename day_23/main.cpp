#include <algorithm>
#include <csignal>
#include <deque>
#include <fstream>
#include <iostream>
#include <mdspan>
#include <ranges>
#include <string>

enum class ETile
{
    path,
    forest,
    slopeUp,
    slopeDown,
    slopeRight,
    slopeLeft
};

ETile charToTile(const char c)
{
    switch (c)
    {
        case '.':
            return ETile::path;
        case '#':
            return ETile::forest;
        case '^':
            return ETile::slopeUp;
        case 'v':
            return ETile::slopeDown;
        case '<':
            return ETile::slopeLeft;
        case '>':
            return ETile::slopeRight;
        default:
            throw std::invalid_argument("Unexpected value");
    }
}

char tileToChar(const ETile t)
{
    switch (t)
    {
        case ETile::path:
            return '.';
        case ETile::forest:
            return '#';
        case ETile::slopeUp:
            return '^';
        case ETile::slopeDown:
            return 'v';
        case ETile::slopeLeft:
            return '<';
        case ETile::slopeRight:
            return '>';
    }
}

using Pos = std::array<size_t, 2>;

constexpr bool
    isValidNext(const Pos& start, const Pos& end, const ETile direction, const bool part1)
{
    if (!part1)
        return direction != ETile::forest;

    switch (direction)
    {
        case ETile::path:
            return true;
        case ETile::forest:
            return false;
        case ETile::slopeUp:
            return end[0] != start[0] + 1;
        case ETile::slopeDown:
            return start[0] != end[0] + 1;
        case ETile::slopeLeft:
            return end[1] != start[1] + 1;
        case ETile::slopeRight:
            return start[1] != end[1] + 1;
    }
}

std::vector<Pos> getRawNeighbours(const auto& maze, const Pos& at)
{
    std::vector<Pos> result;
    for (const auto [i, j]: {
             std::pair{-1, 0 },
             {1,  0 },
             {0,  -1},
             {0,  1 }
    })
    {
        if (i == -1 && at[0] == 0)
            continue;
        if (i == 1 && at[0] == maze.extent(0) - 1)
            continue;
        if (j == -1 && at[1] == 0)
            continue;
        if (j == 1 && at[1] == maze.extent(1) - 1)
            continue;
        result.emplace_back(Pos{at[0] + i, at[1] + j});
    }
    return result;
}

std::vector<Pos>
    neighbours(const auto& maze, const auto& visitedMaze, const Pos& at, const bool part1)
{
    std::vector<Pos> result;
    for (const auto& candidate: getRawNeighbours(maze, at))
    {
        if (isValidNext(at, candidate, maze[candidate], part1) && !visitedMaze[candidate])
            result.emplace_back(std::move(candidate));
    }
    return result;
}

size_t getPathLenght(const auto& maze)
{
    size_t result = 0;
    for (std::size_t i = 0; i != maze.extent(0); i++)
    {
        for (std::size_t j = 0; j != maze.extent(1); j++)
        {
            result += maze[i, j];
        }
    }
    return result;
}

void dfsRec(const auto& maze,
            auto& visitedMaze,
            const Pos& current,
            const Pos& end,
            size_t& maxPath,
            const bool part1)
{
    if (current == end)
    {
        const auto pathLength = getPathLenght(visitedMaze);
        maxPath = std::max(maxPath, pathLength);
        return;
    }

    for (const auto& n: neighbours(maze, visitedMaze, current, part1))
    {
        visitedMaze[n] = 1;
        dfsRec(maze, visitedMaze, n, end, maxPath, part1);
        visitedMaze[n] = 0;
    }
}

size_t dfs(const auto& maze, const Pos& start, const Pos& end, const bool part1)
{
    std::vector<int> visited(maze.extent(0) * maze.extent(1), 0);
    const auto visitedMaze = std::mdspan(visited.data(), maze.extent(0), maze.extent(1));
    visitedMaze[start] = 1;

    size_t occupiedTiles = 0;
    dfsRec(maze, visitedMaze, start, end, occupiedTiles, part1);
    return occupiedTiles - 1;
}

int main()
{
    std::ifstream file("input.txt");
    if (!file)
    {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    std::vector<ETile> data;
    size_t nRows{0};
    size_t nCols{0};
    while (std::getline(file, line))
    {
        if (nCols == 0)
            nCols = line.size();
        ++nRows;
        std::ranges::copy(line | std::views::transform(charToTile), std::back_inserter(data));
    }

    auto maze = std::mdspan(data.data(), nRows, nCols);

    Pos start{
        0,
        static_cast<size_t>(std::distance(std::begin(data), std::ranges::find(data, ETile::path)))};
    Pos end{nRows - 1,
            nCols - 1 -
                (data.size() - static_cast<size_t>(std::distance(
                                   std::find(std::rbegin(data), std::rend(data), ETile::path),
                                   std::rend(data))))};

    std::cout << std::format("Part 1: {}\n", dfs(maze, start, end, true));
    std::cout << std::format("Part 2: {}\n", dfs(maze, start, end, false));
    return 0;
}
