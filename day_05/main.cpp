#include <algorithm>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include "../utils.hpp"

using ulint = unsigned long int;

class RangeMap
{
public:
    ulint keyMin, valMin, range;

    bool contains(ulint key) const { return key >= keyMin && key < keyMin + range; }

    ulint getVal(ulint key) const { return valMin + (key - keyMin); }
};

class TypeFullMap
{
public:
    TypeFullMap() = default;

    void addRange(ulint keyMin, ulint valMin, ulint range)
    {
        ranges.insert(ranges.begin(), {keyMin, valMin, range});
    }

    ulint getValue(ulint key) const
    {
        for (const auto& range : ranges) {
            if (range.contains(key)) {
                printf("key: %lu, val: %lu\n", key, range.getVal(key));
                printf("In range with valStart at: %lu\n", range.valMin);
                return range.getVal(key);
            }
        }
        printf("No found range\n");
        return key;
    }

    void onlyInOneRange(ulint key) const
    {
        int count = 0;
        for (const auto& range : ranges) {
            if (range.contains(key)) {
                count++;
            }
        }
        assert(count == 1 || count == 0);
    }

    void transform(std::vector<ulint>& type) const
    {
        for (auto& val : type) {
            onlyInOneRange(val);
            val = getValue(val);
        }
    }

    void clear() { ranges.clear(); }

private:
    std::vector<RangeMap> ranges;
};


ulint part1()
{
    std::ifstream file("input.txt");
    // std::ifstream file("example.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    std::getline(file, line);
    std::vector<ulint> originalType;
    auto lineSplit = split(split(line, ": ")[1]);
    for (const auto& seed : lineSplit) {
        originalType.push_back(std::stoul(seed));
    }

    std::getline(file, line);
    assert(line == "");

    TypeFullMap typeMap;

    while (std::getline(file, line)) {
        if (line == "") {
            typeMap.transform(originalType);
            typeMap.clear();
            continue;
        }
        if (line.contains("-to-"))
        {
            continue;
        }
        auto lineSplit = split(line);
        assert(lineSplit.size() == 3);
        ulint valueOrigin = std::stoul(lineSplit[0]);
        ulint keyOrigin = std::stoul(lineSplit[1]);
        ulint nElements = std::stoul(lineSplit[2]);
        typeMap.addRange(keyOrigin, valueOrigin, nElements);
    }
    typeMap.transform(originalType);

    return std::ranges::min(originalType);
}

int main() {
    std::printf("Part 1: %lu\n", part1());

    // 69713834 is too low
    return 0;
}
