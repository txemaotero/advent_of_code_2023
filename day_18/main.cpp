#include <fstream>
#include <ranges>
#include <string>
#include <iostream>
#include "../utils.hpp"


struct Vector2D {
    int64 x;
    int64 y;

    Vector2D operator+(const Vector2D& other) const {
        return {x + other.x, y + other.y};
    }

    Vector2D operator*(int scalar) const {
        return {x * scalar, y * scalar};
    }
};

Vector2D getDirVector(const char dir)
{
    switch (dir) {
        case 'R': return {1, 0};
        case 'L': return {-1, 0};
        case 'U': return {0, 1};
        case 'D': return {0, -1};
    }
    return {0, 0};
}

std::pair<Vector2D, uint64> parseLinePart2(const std::string& hexcode)
{
    char dir;
    switch (hexcode[7]) {
        case '0': dir = 'R'; break;
        case '1': dir = 'D'; break;
        case '2': dir = 'L'; break;
        case '3': dir = 'U'; break;
        default:
            std::cerr << "Error parsing hexcode\n";
            return {{0, 0}, 0};
    }
    uint64 steps = std::stoull(hexcode.substr(2, 5), nullptr, 16);
    return {getDirVector(dir), steps};
}


int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;

    Vector2D currentPosPart1 = {0, 0};
    int64 areaPart1 = 0;
    uint64 perimeterPart1 = 0;

    Vector2D currentPosPart2 = {0, 0};
    int64 areaPart2 = 0;
    uint64 perimeterPart2 = 0;
    while (std::getline(file, line)) {
        auto lineSlit = split(line);
        Vector2D dir = getDirVector(lineSlit[0][0]);
        int steps = std::stoi(lineSlit[1]);
        perimeterPart1 += steps;
        Vector2D newPos = currentPosPart1 + dir * steps;
        areaPart1 += newPos.x * currentPosPart1.y - currentPosPart1.x * newPos.y;
        currentPosPart1 = newPos;

        auto [dirPart2, stepsPart2] = parseLinePart2(lineSlit[2]);
        perimeterPart2 += stepsPart2;
        newPos = currentPosPart2 + dirPart2 * stepsPart2;
        areaPart2 += newPos.x * currentPosPart2.y - currentPosPart2.x * newPos.y;
        currentPosPart2 = newPos;
    }
    areaPart1 = (std::abs(areaPart1) + perimeterPart1)/2 + 1;
    std::cout << "Part 1: " << areaPart1 << std::endl;
    areaPart2 = (std::abs(areaPart2) + perimeterPart2)/2 + 1;
    std::cout << "Part 2: " << areaPart2 << std::endl;

    return 0;
}
