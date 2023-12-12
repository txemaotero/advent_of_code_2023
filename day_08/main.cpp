#include <algorithm>
#include <cassert>
#include <concepts>
#include <fstream>
#include <numeric>
#include <string>
#include <ranges>
#include <iostream>
#include <unordered_map>
#include "../utils.hpp"

using ulong = unsigned long long int;


class GameNode
{
public:

    GameNode() = default;

    GameNode(const std::string& code)
        : code(code) {}

    std::string getCode() const { return code; }

    bool operator==(const GameNode& other) const {
        return code == other.code;
    }

    GameNode* getLeft() const { return left; }
    GameNode* getRight() const { return right; }

    GameNode* getConnected(char direction) const {
        if (direction == 'L')
            return left;
        else if (direction == 'R')
            return right;
        else
        {
            std::cerr << "Invalid direction: " << direction << '\n';
            assert(false);
            return nullptr;
        }
    }

    void setConnections(GameNode* left, GameNode* right) {
        this->left = left;
        this->right = right;
    }

    void validateConnections() const {
        if (left == nullptr || right == nullptr)
        {
            std::cout << "Node " << code << " has a null connection\n";
            assert(false);
        }
    }

private:
    std::string code;
    GameNode* left {nullptr};
    GameNode* right {nullptr};
};


struct CycleNode
{
    std::string code;
    int instructionIndex;
    ulong steps;

    bool operator==(const CycleNode& other) const {
        return code == other.code && instructionIndex == other.instructionIndex;
    }
};

std::vector<CycleNode> findCycles(const GameNode* startNode, const std::unordered_map<std::string, GameNode>& nodes, const std::string& instructions)
{
    std::vector<CycleNode> foundCycleNodes;
    ulong steps = 0;
    const int nInstructions = instructions.size();
    int i = 0;
    std::string currentNode = startNode->getCode();
    while (true)
    {
        if (currentNode[2] == 'Z')
        {
            CycleNode cycleNode {currentNode, i, steps};
            bool visited = std::find(foundCycleNodes.begin(), foundCycleNodes.end(), cycleNode) != foundCycleNodes.end();
            foundCycleNodes.push_back(cycleNode);
            if (visited)
            {
                return foundCycleNodes;
            }
        }
        char direction = instructions[i];
        currentNode = nodes.at(currentNode).getConnected(direction)->getCode();
        steps++;
        i++;
        if (i >= nInstructions)
            i = 0;
    }
}

ulong lcm(ulong a, ulong b) { return (a / std::gcd(a, b)) * b; }


int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::unordered_map<std::string, GameNode> nodes;

    std::string line;
    std::getline(file, line);
    std::string instructions = line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        auto lineSpl = split(line, " = ");
        std::string code = lineSpl[0];
        GameNode node {code};
        nodes[code] = node;
        std::string leftRight = lineSpl[1];
        leftRight.replace(0, 1, "");
        leftRight.replace(leftRight.size() - 1, 1, "");
        auto leftRightSpl = split(leftRight, ", ");
        std::string leftCode = leftRightSpl[0];
        std::string rightCode = leftRightSpl[1];
        if (!nodes.contains(leftCode))
            nodes[leftCode] = GameNode {leftCode};
        if (!nodes.contains(rightCode))
            nodes[rightCode] = GameNode {rightCode};
        nodes[code].setConnections(&(nodes[leftCode]), &(nodes[rightCode]));
    }

    // Validate
    for (const auto& [_, node] : nodes) {
        node.validateConnections();
    }

    unsigned long long int steps = 0;
    int nInstructions = instructions.size();
    int i = 0;
    std::string currentNode = "AAA";

    while (true)
    {
        if (currentNode == "ZZZ")
            break;
        char direction = instructions[i];
        currentNode = nodes[currentNode].getConnected(direction)->getCode();
        steps++;
        i++;
        if (i >= nInstructions)
            i = 0;
    }
    std::cout << "Part 1: " << steps << '\n';

    // Part 2
    steps = 0;
    i = 0;
    ulong minimumSteps = 1;
    for (const auto& [_, node] : nodes) {
        if (node.getCode()[2] == 'A')
        {
            auto cycle = findCycles(&node, nodes, instructions);
            assert(cycle[1].steps == cycle[0].steps*2);
            minimumSteps = lcm(minimumSteps, cycle[0].steps);
        }
    }

    std::cout << "Part 2: " << minimumSteps << '\n';
    return 0;
}
