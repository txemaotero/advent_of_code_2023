#include <cmath>
#include <format>
#include <fstream>
#include <numeric>
#include <optional>
#include <string>
#include <iostream>
#include "../utils.hpp"
#include <stdfloat>


using int_t = long long;
using myF_t = std::float128_t;

using Vec2d = std::array<int_t, 2>;
using Vec2df = std::array<myF_t, 2>;

template<typename T>
concept Vec = std::is_same_v<T, Vec2d> || std::is_same_v<T, Vec2df>;

template<Vec T>
auto dot(const T& v1, const T& v2)
{
    return std::inner_product(begin(v1), end(v1), begin(v2), 0);
}

template<Vec T>
auto cross(const Vec auto& v1, const Vec auto& v2)
{
    return v1[0] * v2[1] - v1[1] * v2[0];
}

template<Vec T>
T operator+(const T& v1, const T& v2)
{
    return {v1[0] + v2[0], v1[1] + v2[1]};
}

template<Vec T>
T operator-(const T& v1, const T& v2)
{
    return {v1[0] - v2[0], v1[1] - v2[1]};
}

template<Vec T>
T operator*(const T& v1, const typename T::value_type s)
{
    return {v1[0] * s, v1[1] * s};
}

template<Vec T>
T operator*(const typename T::value_type s, const Vec auto& v1)
{
    return v1 * s;
}

Vec2df toF(const Vec2d& v) 
{
    return {static_cast<myF_t>(v[0]), static_cast<myF_t>(v[1])};
}

struct Line
{
    Vec2d point;
    Vec2d director;

    std::optional<Vec2df> intersection(const Line& other) const
    {
        const auto cross = director[0] * other.director[1] - director[1] * other.director[0];
        if (!cross)
            return std::nullopt;

        const Vec2d delta = other.point - point;
        const myF_t s = (delta[0] * other.director[1] - delta[1] * other.director[0]) /
                         static_cast<myF_t>(cross);
        return toF(point) + toF(director) * s;
    }

    bool willGo(const Vec2d& p) const
    {
        const auto delta = p - point;
        return dot(delta, director) > 0;
    }

    bool willGo(const Vec2df& p) const
    {
        const auto delta = p - toF(point);
        return dot(delta, toF(director)) > 0;
    }
};

struct Box
{
    int_t min, max;

    bool contains(const Vec2df& v)const
    {
        const auto& [x, y] = v;
        return x >= min && x <= max && y >= min && y <= max;
    }
};

int main() {
    // NOTE: Compile with g++
    std::ifstream file("input.txt");
    const int_t MIN = 200000000000000;
    const int_t MAX = 400000000000000;
    // std::ifstream file("example.txt");
    // const int_t MIN = 7;
    // const int_t MAX = 27;
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::vector<Line> lines;
    std::string line;
    while (std::getline(file, line))
    {
        const auto [positionStr, velocityStr] = split_once(line, " @ ");
        const auto posSpl = split(positionStr, ", ");
        Vec2d pos{std::stoll(posSpl[0]), std::stoll(posSpl[1])};
        const auto velSpl = split(velocityStr, ", ");
        Vec2d vel{std::stoll(velSpl[0]), std::stoll(velSpl[1])};
        lines.push_back({pos, vel});
    }

    const Box box {MIN, MAX};
    unsigned part1result{0};
    for (auto i = 0; i < lines.size() - 1; ++i)
    {
        for (auto j = i + 1; j < lines.size(); ++j)
        {
            const auto& l1 = lines[i];
            const auto& l2 = lines[j];
            const auto inters = l1.intersection(l2);
            if (!inters)
                continue;

            if (!l1.willGo(*inters) || !l2.willGo(*inters))
                continue;

            if (box.contains(*inters))
                ++part1result;
        }
    }
    std::cout << std::format("Part 1: {}\n", part1result);

    return 0;
}
