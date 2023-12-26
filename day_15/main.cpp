#include <fstream>
#include <print>
#include <string>
#include <iostream>
#include "../utils.hpp"


uint64 hash(const std::string& step) {
    uint64 hash = 0;
    for (auto c : step) {
        hash += c;
        hash *= 17;
        hash %= 256;
    }
    return hash;
}

class Lens {
public:
    std::string label;
    uint32 focal_length;

    bool operator==(const Lens& other) const {
        return label == other.label;
    }
};

class Box {
public:
    void addLens(const Lens& lens) {
        auto it = std::find(lenses.begin(), lenses.end(), lens);
        if (it == lenses.end()) {
            lenses.push_back(lens);
        } else {
            *it = lens;
        }
    }

    void removeLens(const Lens& lens) {
        auto it = std::find(lenses.begin(), lenses.end(), lens);
        if (it != lenses.end()) {
            lenses.erase(it);
        }
    }

    uint64 focusingPower() const {
        uint64 power = 0;
        for (uint32 i = 0; auto lens : lenses) {
            power += lens.focal_length * (++i) * (id + 1);
        }
        return power;
    }

    void setId(uint32 id) {
        this->id = id;
    }

private:
    std::vector<Lens> lenses;
    uint32 id;
};

int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    std::getline(file, line);
    auto steps = split(line, ",");
    std::array<Box, 256> boxes;
    for (uint32 i = 0; i < 256; ++i) {
        boxes[i].setId(i);
    }
    uint64 part1 = 0;
    for (auto step : steps) {
        part1 += hash(step);
        if (step.contains("="))
        {
            auto elemsSplit = split(step, "=");
            auto boxId = hash(elemsSplit[0]);
            uint32 lensFocal = std::stoi(elemsSplit[1]);
            boxes[boxId].addLens({elemsSplit[0], lensFocal});
        }
        else
        {
            step.pop_back();
            auto boxId = hash(step);
            boxes[boxId].removeLens({step, 0});
        }
    }
    std::println("Part 1: {}", part1);
    uint64 part2 = 0;
    for (auto box : boxes) {
        part2 += box.focusingPower();
    }
    std::println("Part 2: {}", part2);
    return 0;
}
