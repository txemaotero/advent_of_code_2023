#include <cassert>
#include <cmath>
#include <concepts>
#include <fstream>
#include <array>
#include <limits>
#include <vector>
#include <string>
#include <iostream>
#include "../utils.hpp"


template <std::integral T>
std::array<T, 2> findWaitTimeLimits(T availableTime, T minDistance)
{
    if (availableTime < 2*std::sqrt(minDistance)) {
        std::cerr << "No solution\n";
        return {0, 0};
    }
    float availableTimeSq = static_cast<float>(availableTime);
    availableTimeSq *= availableTimeSq;
    float discr = std::sqrt(availableTimeSq - 4*minDistance);
    T lower = std::ceil((availableTime - discr) / 2.f);
    T upper = std::floor((availableTime + discr) / 2.f);
    T lowerVal = (availableTime - lower) * lower;
    // Adjust the result due to floating point errors
    while((availableTime - lower) * lower <= minDistance) { ++lower; }
    while((availableTime - upper) * upper <= minDistance) { --upper; }
    assert(lower <= upper);
    return {lower, upper};
}

int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    std::vector<int> times;
    std::vector<int> distances;

    std::getline(file, line);
    auto timesStrs = split(strip(split(line, ":")[1]));
    std::getline(file, line);
    auto distStrs = split(strip(split(line, ":")[1]));
    std::string part2TotalTimeStr {};
    std::string part2TotalDistStr {};

    int part1 = 1;
    for (int i = 0; i < timesStrs.size(); ++i) {
        part2TotalTimeStr += timesStrs[i];
        part2TotalDistStr += distStrs[i];
        int time = std::stoi(timesStrs[i]);
        int dist = std::stoi(distStrs[i]);
        times.push_back(time);
        distances.push_back(dist);
        auto [low, high] = findWaitTimeLimits(time, dist);
        part1 *= high - low + 1;
    }
    std::cout << "Part 1: " << part1 << '\n';

    unsigned long long part2 = 0;
    unsigned long long totalTime = std::stoull(part2TotalTimeStr);
    unsigned long long totalDist = std::stoull(part2TotalDistStr);

    auto [low, high] = findWaitTimeLimits(totalTime, totalDist);
    part2 = high - low + 1;
    std::cout << "Part 2: " << part2 << '\n';
    return 0;
}
