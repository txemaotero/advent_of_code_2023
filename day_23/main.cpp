#include <algorithm>
#include <csignal>
#include <deque>
#include <fstream>
#include <numeric>
#include <ranges>
#include <mdspan>
#include <string>
#include <iostream>


enum class ETile
{
    path, forest, slopeUp, slopeDown, slopeRight, slopeLeft
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

using Pos = std::array<size_t, 2>;

size_t distance(const Pos& p1, const Pos& p2)
{
    return std::inner_product(begin(p1), end(p1), begin(p2), 0);
}

struct State
{
    Pos at;
    std::vector<Pos> visited;

    size_t pathLength() const
    {
        return visited.size() - 1;
    }

    State getNew(const Pos& newPos)
    {
        auto v = visited;
        v.push_back(newPos);
        return {newPos, v};
    }

    size_t quality(const Pos& target) const
    {
        return pathLength() + (~size_t{} - distance(at, target));
    }
};

constexpr bool isValidNext(const Pos& start, const Pos& end, const ETile direction, const bool part1)
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

std::vector<Pos> neighbours(const auto& maze, const State& current, const bool part1)
{
    std::vector<Pos> result;
    const auto& at = current.at;
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
        Pos candidate{at[0] + i, at[1] + j};
        if (!std::ranges::contains(current.visited, candidate) &&
            isValidNext(at, candidate, maze[candidate], part1))
            result.emplace_back(std::move(candidate));
    }
    return result;
}

size_t stepsInLongesPath(const auto& maze, const Pos& start, const Pos& end, const bool part1)
{
    const auto compareStates = [&end](const State& lhs, const State& rhs)
    {
        return lhs.quality(end) < rhs.quality(end);
    };
    State current{start, {start}};
    std::vector toCheck{current};
    while (!toCheck.empty())
    {
        auto checking = std::move(toCheck.back());
        toCheck.pop_back();
        if (checking.at == end)
            return checking.pathLength();
        auto neigs = neighbours(maze, checking, part1);
        std::ranges::for_each(neigs,
                              [&checking, &toCheck, &compareStates, &end](const auto& neig)
                              {
                                  // std::cout << std::format("{}, {}\n", neig[0], neig[1]);
                                  const auto newToCheck = checking.getNew(neig);
                                  const auto it =
                                      std::ranges::lower_bound(toCheck, newToCheck, compareStates);
                                  if (it == std::end(toCheck) || it->at != newToCheck.at)
                                      toCheck.insert(it, newToCheck);
                                  else if (it->quality(end) < newToCheck.quality(end))
                                      *it = newToCheck;
                              });
    }
    throw std::invalid_argument("Solution not found");
}

int main() {
    std::ifstream file("input.txt");
    // std::ifstream file("example.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    std::vector<ETile> data;
    size_t nRows {0};
    size_t nCols {0};
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
            nCols - 1 - (data.size() - static_cast<size_t>(std::distance(
                                       std::find(std::rbegin(data), std::rend(data), ETile::path),
                                       std::rend(data))))};

    std::cout << std::format("Part 1: {}\n", stepsInLongesPath(maze, start, end, true));
    std::cout << std::format("Part 2: {}\n", stepsInLongesPath(maze, start, end, false));
    return 0;
}
