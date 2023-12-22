#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include "../utils.hpp"


struct pair_hash {
    std::size_t operator () (const std::pair<size_t, size_t> &p) const {
        return p.first << 32 | p.second;
    }
};

using Cache = std::unordered_map<std::pair<size_t, size_t>, uint64, pair_hash>;


bool moveHashForward(const std::string& sequence, size_t& position, size_t nHashes)
{
    while (nHashes > 0 && position < sequence.size())
    {
        if (sequence[position] == '.')
        {
            return false;
        }
        position++;
        nHashes--;
    }
    if (nHashes == 0 && position < sequence.size())
        return sequence[position] != '#';
    return nHashes == 0;
};


uint64 evalOneMoreChar(
        const std::string& sequence,
        size_t position,
        const std::vector<size_t>& target,
        size_t currentTargetPos,
        Cache& cache)
{
    auto key = std::make_pair(position, currentTargetPos);
    if (cache.contains(key))
        return cache[key];

    while (position < sequence.size() && sequence[position] != '?')
    {
        if (sequence[position] == '.')
        {
            position++;
        }
        else
        {
            if (!moveHashForward(sequence, position, target[currentTargetPos]))
            {
                cache[key] = 0;
                return 0;
            }
            currentTargetPos++;
            if (position < sequence.size() && sequence[position] == '?')
                position++;
        }
    }
    if (position == sequence.size())
    {
        uint64 result;
        if (currentTargetPos == target.size())
        {
            result = 1;
        }
        else
        {
            result = 0;
        }
        cache[key] = result;
        return result;
    }
    if (sequence[position] == '?')
    {
        uint64 branchDot = evalOneMoreChar(sequence, 1+position, target, currentTargetPos, cache);
        if (!moveHashForward(sequence, position, target[currentTargetPos]))
        {
            cache[key] = branchDot;
            return branchDot;
        }
        currentTargetPos++;
        if (sequence[position] == '?')
            position++;
        uint64 branchHash = evalOneMoreChar(sequence, position, target, currentTargetPos, cache);
        cache[key] = branchDot + branchHash;
        return branchDot + branchHash;
    }
    cache[key] = 0;
    return 0;
}


std::pair<std::string, std::vector<size_t>> modifyInputPart2(const std::string& sequence, const std::vector<size_t>& target)
{
    std::string newSequence = sequence;
    std::vector<size_t> newTarget = target;
    for (int i = 0; i < 4; i++)
    {
        newSequence += '?';
        newSequence += sequence;
        newTarget.insert(newTarget.end(), target.begin(), target.end());
    }
    return std::make_pair(newSequence, newTarget);
}


int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    uint64 part1 = 0;
    uint64 part2 = 0;
    std::string line;
    Cache cache;
    while (std::getline(file, line)) {
        std::vector<std::string> lineSpl = split(line);
        std::string sequence = lineSpl[0];
        auto valuesStrs = split(lineSpl[1], ",");
        std::vector<size_t> target;
        for (const auto& valStr : valuesStrs)
        {
            target.push_back(std::stoi(valStr));
        }
        cache.clear();
        part1 += evalOneMoreChar(sequence, 0, target, 0, cache);

        auto [sequence2, target2] = modifyInputPart2(sequence, target);
        cache.clear();
        part2 += evalOneMoreChar(sequence2, 0, target2, 0, cache);
    }

    std::cout << "Part 1: " << part1 << std::endl;
    std::cout << "Part 2: " << part2 << std::endl;
    return 0;
}
