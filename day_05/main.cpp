#include <algorithm>
#include <cassert>
#include <array>
#include <cstdio>
#include <fstream>
#include <optional>
#include <string>
#include <iostream>
#include <vector>
#include <ranges>
#include "../utils.hpp"

using ulint = unsigned long long int;

const char* INPUT_FILE = "input.txt";

class Range
{
public:
    ulint start, range;

    ulint end() const { return start + range; }

    bool contains(ulint key) const { return key >= start && key < end(); }

    std::optional<Range> intersection(const Range& other) const
    {
        ulint start = std::max(this->start, other.start);
        ulint end = std::min(this->end(), other.end());
        if (start < end) {
            return Range{start, end - start};
        }
        return std::nullopt;
    }
};

class RangeMap
{
public:
    ulint keyMin, valMin, range;

    bool contains(ulint key) const { return key >= keyMin && key < keyMin + range; }

    Range getKeyRange() const { return {keyMin, range}; }

    ulint getVal(ulint key) const { return valMin + (key - keyMin); }

    std::array<Range, 3> getVal(const Range& keyRange) const
    {
        const Range currentRange = getKeyRange();
        const auto intersection = currentRange.intersection(keyRange);
        assert(intersection.has_value());
        const Range& intersectionRange = *intersection;
        std::array<Range, 3> result;
        result[0] = {keyRange.start, intersectionRange.start - keyRange.start};
        result[1] = {getVal(intersectionRange.start), intersectionRange.range};
        result[2] = {intersectionRange.end(), keyRange.end() - intersectionRange.end()};
        return result;
    }

};

std::ostream& operator<<(std::ostream& os, const Range& range)
{
    return os << "[" << range.start << ", " << range.end() << ")";
}

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
                return range.getVal(key);
            }
        }
        return key;
    }

    std::vector<Range> getValue(const Range& keyRange) const
    {
        std::vector<Range> result;
        std::vector<Range> remainingRanges = {keyRange};

        while (remainingRanges.size())
        {
            Range currentRange = remainingRanges.back();
            remainingRanges.pop_back();
            bool hasIntersection = false;
            for (const auto& range : ranges) {
                if (!range.getKeyRange().intersection(currentRange)) {
                    continue;
                }
                hasIntersection = true;
                const auto subRanges = range.getVal(currentRange);
                if (subRanges[0].range)
                {
                    remainingRanges.push_back(subRanges[0]);
                }
                if (subRanges[2].range)
                {
                    remainingRanges.push_back(subRanges[2]);
                }
                result.push_back(subRanges[1]);
                break;
            }
            if (!hasIntersection) {
                result.push_back(currentRange);
            }
        }
        return result;
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

    void transform(std::vector<Range>& typeRanges) const
    {
        std::vector<Range> result;
        for (const auto& valRange : typeRanges) {
            std::vector<Range> mappedRange = getValue(valRange);
            result.insert(result.end(), mappedRange.begin(), mappedRange.end());
        }
        typeRanges = result;
    }

    void clear() { ranges.clear(); }

private:
    std::vector<RangeMap> ranges;
};


ulint part1()
{
    std::ifstream file(INPUT_FILE);
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

ulint part2()
{
    std::ifstream file(INPUT_FILE);
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    std::getline(file, line);
    std::vector<Range> seedRanges;
    auto lineSplit = split(split(line, ": ")[1]);
    int seedPairs = lineSplit.size() / 2;
    for (int i = 0; i < seedPairs; ++i) {
        ulint start = std::stoul(lineSplit[i * 2]);
        ulint range = std::stoul(lineSplit[i * 2 + 1]);
        seedRanges.push_back({ start, range });
    }

    std::getline(file, line);
    assert(line == "");

    TypeFullMap typeMap;

    while (std::getline(file, line)) {
        if (line == "") {
            typeMap.transform(seedRanges);
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
    typeMap.transform(seedRanges);

    return std::ranges::min(std::ranges::views::all(seedRanges) | std::views::transform([](auto& r) { return r.start; }));
}

int main() {
    std::printf("Part 1: %llu\n", part1());
    std::printf("Part 2: %llu\n", part2());

    // 69713834 is too low
    return 0;
}
