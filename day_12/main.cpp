#include <algorithm>
#include <cassert>
#include <fstream>
#include <numeric>
#include <vector>
#include <string>
#include <iostream>
#include "../utils.hpp"



void evalOneMoreChar(std::string sequence, size_t position, size_t consecutives,
        size_t currentTargetPos, size_t missingInterr,
        size_t passedOks,
        size_t missingOks, size_t totalOk,
        const std::vector<size_t>& target, int& totalValids)
{
    // std::cout << "Called with " << sequence << " " << position << " " << consecutives << " " << currentTargetPos << std::endl;
    if (position == sequence.size())
    {
        if ((consecutives > 0 && target[currentTargetPos] == consecutives && currentTargetPos == target.size() - 1) || currentTargetPos == target.size())
        {
            // std::cout << sequence << std::endl;
            totalValids++;
        }
        return;
    }
    if (sequence[position] == '?')
    {
        missingInterr--;
        sequence[position] = '.';
        evalOneMoreChar(sequence, position, consecutives, currentTargetPos, passedOks, missingOks, totalOk, missingInterr, target, totalValids);
        sequence[position] = '#';
        evalOneMoreChar(sequence, position, consecutives, currentTargetPos, passedOks, missingOks, totalOk, missingInterr, target, totalValids);
    }
    else if (sequence[position] == '.')
    {
        if (consecutives > 0)
        {
            if (target[currentTargetPos] != consecutives)
                return;
            currentTargetPos++;
        }
        consecutives = 0;
        evalOneMoreChar(sequence, ++position, consecutives, currentTargetPos, passedOks, missingOks, totalOk, missingInterr, target, totalValids);
    }
    else if (sequence[position] == '#')
    {
        if (currentTargetPos == target.size() || consecutives == target[currentTargetPos])
        {
            return;
        }
        passedOks++;
        missingOks--;
        if (passedOks > totalOk || passedOks + missingOks + missingInterr < totalOk)
        {
            return;
        }
        consecutives++;
        evalOneMoreChar(sequence, ++position, consecutives, currentTargetPos, passedOks, missingOks, totalOk, missingInterr, target, totalValids);
    }
    else
    {
        assert(false);
    }

}

std::string modifySequencePart2(const std::string& sequence)
{
    std::string newSequence = sequence;
    for (int i = 0; i < 4; i++)
    {
        newSequence += '?';
        newSequence += sequence;
    }
    return newSequence;
}

std::vector<size_t> modifyTargetPart2(const std::vector<size_t>& target)
{
    std::vector<size_t> newTarget = target;
    for (int i = 0; i < 4; i++)
    {
        newTarget.insert(newTarget.end(), target.begin(), target.end());
    }
    return newTarget;
}

int main() {
    // std::ifstream file("input.txt");
    std::ifstream file("example.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    int part1 = 0;
    unsigned long long int part2 = 0;
    std::string line;
    while (std::getline(file, line)) {
        int valids = 0;
        std::vector<std::string> lineSpl = split(line);
        std::string sequence = lineSpl[0];
        size_t missingInterr = std::count(sequence.begin(), sequence.end(), '?');
        size_t missingOks = std::count(sequence.begin(), sequence.end(), '#');
        auto valuesStrs = split(lineSpl[1], ",");
        std::vector<size_t> target;
        for (const auto& valStr : valuesStrs)
        {
            target.push_back(std::stoi(valStr));
        }
        size_t totalValids = std::accumulate(target.begin(), target.end(), 0);
        evalOneMoreChar(sequence, 0, 0, 0, missingInterr, 0, missingOks, totalValids, target, valids);
        part1 += valids;

        auto sequence2 = modifySequencePart2(sequence);
        auto target2 = modifyTargetPart2(target);
        valids = 0;
        evalOneMoreChar(sequence2, 0, 0, 0, missingInterr, 0, missingOks, totalValids, target2, valids);
        part2 += valids;
        std::cout << "Line" << std::endl;
    }

    std::cout << "Part 1: " << part1 << std::endl;
    std::cout << "Part 2: " << part2 << std::endl;
    return 0;
}
