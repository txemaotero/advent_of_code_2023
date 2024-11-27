#include <format>
#include <fstream>
#include <limits>
#include <map>
#include <ranges>
#include "../utils.hpp"
#include <set>
#include <string>
#include <iostream>
#include <vector>


struct Node
{
    Node(const size_t i) : index(i) {}

    size_t index;
    std::vector<Node*> connections;
};

void connect(Node& n1, Node& n2)
{
    n1.connections.push_back(&n2);
    n2.connections.push_back(&n1);
}

using AdjacencyMatrix = std::vector<std::vector<int>>;

int stoerWagnerMinCut(const std::vector<std::vector<int>> &originalGraph, std::vector<int> &subsetA) {
    int n = originalGraph.size();
    std::vector<std::vector<int>> graph = originalGraph; // Make a copy since we'll modify it
    std::vector<std::set<int>> vertexSets(n); // To track the merged vertices
    std::vector<int> vertices(n);

    // Initialize the list of vertices and vertex sets
    for (int i = 0; i < n; ++i) {
        vertices[i] = i;
        vertexSets[i].insert(i);
    }

    int minCut = std::numeric_limits<int>::max();

    // While more than one vertex remains
    while (n > 1) {
        std::vector<int> weights(n, 0);
        std::vector<bool> used(n, false);
        int prev = -1;
        int last = -1;

        // Add vertices one by one
        for (int i = 0; i < n; ++i) {
            int sel = -1;
            // Select the next vertex with the maximum weight
            for (int j = 0; j < n; ++j) {
                if (!used[j] && (sel == -1 || weights[j] > weights[sel]))
                    sel = j;
            }

            if (i == n - 1) {
                // Update the minimum cut value
                if (weights[sel] < minCut) {
                    minCut = weights[sel];
                    // Store the partition corresponding to the current minimum cut
                    subsetA.clear();
                    subsetA.insert(subsetA.end(), vertexSets[sel].begin(), vertexSets[sel].end());
                }

                // Merge the last two vertices
                for (int j = 0; j < n; ++j) {
                    if (j != sel && j != prev) {
                        graph[vertices[prev]][vertices[j]] += graph[vertices[sel]][vertices[j]];
                        graph[vertices[j]][vertices[prev]] = graph[vertices[prev]][vertices[j]];
                    }
                }

                // Merge the vertex sets
                vertexSets[prev].insert(vertexSets[sel].begin(), vertexSets[sel].end());

                // Remove the last vertex
                vertices.erase(vertices.begin() + sel);
                vertexSets.erase(vertexSets.begin() + sel);
                n--;
                break;
            }

            used[sel] = true;
            // Update the weights of the connected vertices
            for (int j = 0; j < n; ++j) {
                if (!used[j])
                    weights[j] += graph[vertices[sel]][vertices[j]];
            }
            prev = sel;
        }
    }
    return minCut;
}

int main()
{
    std::ifstream file("input.txt");
    if (!file)
    {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::map<std::string, Node> nodes;
    std::string line;
    size_t index{0};
    while (std::getline(file, line))
    {
        const auto [n1, cons] = split_once(line, ": ");
        const auto connections = split(cons);
        const auto [n1It, inserted] = nodes.try_emplace(n1, index);
        if (inserted)
            ++index;
        std::ranges::for_each(connections,
                              [n1It, &nodes, &index](auto& n2)
                              {
                                  auto [n2It, inserted] = nodes.try_emplace(n2, index);
                                  if (inserted)
                                      ++index;
                                  connect(n1It->second, n2It->second);
                              });
    }
    AdjacencyMatrix mat(nodes.size(), std::vector<int>(nodes.size(), 0));
    std::ranges::for_each(nodes,
                          [&mat](const auto& p)
                          {
                              for (const auto& p2: p.second.connections)
                              {
                                  mat[p2->index][p.second.index] = 1;
                                  mat[p.second.index][p2->index] = 1;
                              }
                          });

    std::vector<int> subsetA;
    stoerWagnerMinCut(mat, subsetA);
    auto subB = mat.size() - subsetA.size();
    std::cout << std::format("Part 1: {}\n", subB * subsetA.size());
    return 0;
}
