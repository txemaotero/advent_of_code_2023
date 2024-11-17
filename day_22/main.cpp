#include <format>
#include <fstream>
#include <functional>
#include <ranges>
#include <set>
#include <string>
#include <iostream>
#include "../utils.hpp"


class Range
{
public:
    Range() = default;
    Range(const size_t v1, const size_t v2):
        min{std::min(v1, v2)},
        max{std::max(v1, v2)}
    {}

    size_t getMin() const
    {
        return min;
    }

    size_t getMax() const
    {
        return max;
    }

    Range& operator-=(const size_t n)
    {
        if (n >= min)
        {
            throw std::invalid_argument("Large n");
        }
        min -= n;
        max -= n;
        return *this;
    }

    bool intersects(const Range& other) const
    {
        return !(max < other.min || min > other.max);
    }
private:
    size_t min, max;
};

struct Brick
{
    Brick(const std::string& b, const std::string& e)
    {
        const auto bsplit = split(b, ",");
        const auto esplit = split(e, ",");
        xr = {std::stoul(bsplit[0]), std::stoul(esplit[0])};
        yr = {std::stoul(bsplit[1]), std::stoul(esplit[1])};
        zr = {std::stoul(bsplit[2]), std::stoul(esplit[2])};
    }

    void down(const size_t n = 1)
    {
        zr -= n;
    }

    bool supports(const Brick& other) const
    {
        return other.zr.getMin() == zr.getMax() + 1 && hasXYOverlap(other);
    }

    bool hasXYOverlap(const Brick& other) const
    {
        return xr.intersects(other.xr) && yr.intersects(other.yr);
    }

    Range xr, yr, zr;
};

class Bricks
{
public:
    void push(Brick&& brick)
    {
        const auto it = std::lower_bound(std::begin(sortedBricks),
                                         std::end(sortedBricks),
                                         brick,
                                         [](const auto& l, const auto& r)
                                         {
                                             return l.zr.getMin() > r.zr.getMin();
                                         });
        sortedBricks.insert(it, std::move(brick));
    }

    std::optional<Brick> popLower()
    {
        if (sortedBricks.empty())
            return {};
        auto result = sortedBricks.back();
        sortedBricks.pop_back();
        return result;
    }

private:
    std::vector<Brick> sortedBricks;
};

class PlacedBricks
{
public:
    static constexpr size_t FLOOR_ID = ~0;

    static constexpr std::array letters {'A', 'B', 'C', 'D', 'E', 'F', 'G'};

    void insert(Brick&& brick)
    {
        Connects connectsTo;
        while (true)
        {
            if (brick.zr.getMin() == 1)
            {
                connectsTo.push_back(FLOOR_ID);
                break;
            }
            std::ranges::for_each(std::ranges::enumerate_view(placedBricks),
                                  [&connectsTo, &brick](const auto& p)
                                  {
                                      const auto& [i, b] = p;
                                      if (b.supports(brick))
                                          connectsTo.push_back(i);
                                  });
            if (!connectsTo.empty())
                break;
            brick.down();
        }
        placedBricks.emplace_back(std::move(brick));
        connections.emplace_back(std::move(connectsTo));
    }

    using Connects = std::vector<std::size_t>;

    size_t howManyCanBeRemoved() const
    {
        // i can be removed if it is not the only support for any other
        std::set<size_t> notRemovables;
        for (const auto& con: connections)
        {
            if (con.empty())
                throw std::invalid_argument("Bad number of connections");
            if (con.size() == 1 && con[0] != FLOOR_ID)
                notRemovables.insert(con[0]);
        }
        return placedBricks.size() - notRemovables.size();
    }

    void printStatus() const
    {
        std::ranges::for_each(
            std::ranges::enumerate_view(connections),
            [](const auto& p)
            {
                const auto& [i, c] = p;
                std::cout << std::format("{} is supported by: {}\n", letters[i%letters.size()], join(c, ", "));
            });
    }

    size_t howManyAreRemovedIfDesintegrate(const size_t i) const
    {
        std::vector<size_t> toBeRemoved{i};
        size_t result{0};
        auto connectionsCopy{connections};
        while (!toBeRemoved.empty())
        {
            auto removing = toBeRemoved.back();
            toBeRemoved.pop_back();
            std::ranges::for_each(std::ranges::enumerate_view(connectionsCopy),
                                  [&result, &toBeRemoved, removing](auto iConn)
                                  {
                                      auto& [i, conn] = iConn;
                                      const auto it = std::ranges::find(conn, removing);
                                      if (it != std::end(conn))
                                      {
                                          conn.erase(it);
                                          if (conn.empty())
                                          {
                                              ++result;
                                              toBeRemoved.push_back(i);
                                          }
                                      }
                                  });
        }
        return result;
    }

    size_t part2() const
    {
        return std::ranges::fold_left(
            std::ranges::iota_view{ 0u, connections.size() } |
                std::views::transform(std::bind(&PlacedBricks::howManyAreRemovedIfDesintegrate,
                                                *this,
                                                std::placeholders::_1)),
            0,
            std::plus{});
    }

private:
    std::vector<Brick> placedBricks;
    std::vector<Connects> connections;
    size_t maxHeight {0};
};

int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    Bricks bricks;
    std::string line;
    while (std::getline(file, line))
    {
        const auto [b, e] = split_once(line, "~");
        bricks.push({b, e});
    }

    PlacedBricks placedBricks;
    while (auto b = bricks.popLower())
        placedBricks.insert(std::move(*b));

    std::cout << std::format("Part 1: {}\n", placedBricks.howManyCanBeRemoved());
    std::cout << std::format("Part 2: {}\n", placedBricks.part2());

    return 0;
}
