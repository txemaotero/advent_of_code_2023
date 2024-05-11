/**
 * Idea Parte 2: El workflow es un árbol binario. Las hojas serán A o R y los nodos guardarán un
 * valor que sirve para saber dado un input si se tiene que ir a la rama de menores o de mayores.
 */
#include "../utils.hpp"
#include <range/v3/all.hpp>

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <variant>

namespace rv = ranges::views;
namespace rg = ranges;

/////////// PART 1 UTILS //////////////

struct Part
{
    uint32 x;
    uint32 m;
    uint32 a;
    uint32 s;

    uint32 points() const
    {
        return x + m + a + s;
    }

    uint32 getProp(char p) const
    {
        switch (p)
        {
            case 'x':
                return x;
            case 'm':
                return m;
            case 'a':
                return a;
            case 's':
                return s;
            default:
                assert(false);
                return 0;
        }
    }
};

using Workflow = std::function<std::string(const Part&)>;

Workflow build_workflow(const std::vector<std::string>& commands)
{
    return [commands](const Part& part)
    {
        for (const auto& command: commands)
        {
            if (!command.contains(':'))
            {
                return command;
            }
            auto split_command = split(command, ":");
            assert(split_command.size() == 2);
            const std::string& condition = split_command.front();
            const std::string& result = split_command.back();
            std::function<bool(uint32, uint32)> operation;
            std::string split_str;
            if (condition.contains('<'))
            {
                operation = std::less{};
                split_str = "<";
            }
            else if (condition.contains('>'))
            {
                operation = std::greater{};
                split_str = ">";
            }
            auto split_condition = split(condition, split_str);
            assert(split_condition.size() == 2);
            const std::string& prop = split_condition.front();
            assert(prop.size() == 1);
            const int value = std::stoi(split_condition.back());
            if (operation(part.getProp(prop[0]), value))
            {
                return result;
            }
        }
        assert(false);
        return std::string("null_result");
    };
}

class PartsAnalyazer
{
public:
    PartsAnalyazer(const std::map<std::string, Workflow>& workflows):
        mWorkflows{workflows}
    {}

    bool operator()(const Part& part)
    {
        std::string currentLabel = "in";
        while (true)
        {
            const Workflow& workflow = mWorkflows[currentLabel];
            currentLabel = workflow(part);
            if (currentLabel == "A")
            {
                return true;
            }
            else if (currentLabel == "R")
            {
                return false;
            }
            else if (currentLabel == "null_result")
            {
                assert(false);
                return false;
            }
        }
    }

private:
    std::map<std::string, Workflow> mWorkflows;
};

/////////// PART 2 UTILS //////////////

struct Interval
{
    uint32 begin;
    uint32 end;

    uint32 n_elements() const
    {
        return end - begin;
    }

    bool contains(uint32 v) const
    {
        return begin <= v && v < end;
    }

    std::optional<std::pair<Interval, Interval>> split(uint32 v) const
    {
        if (!contains(v))
        {
            return {};
        }
        return {std::make_pair(Interval{begin, v}, Interval{v, end})};
    }
};

struct IntervalList
{
    std::vector<Interval> intervals;

    IntervalList():
        intervals{
            Interval{1, 4001}
    }
    {}

    IntervalList(const std::vector<Interval>& ivs):
        intervals{ivs}
    {}

    uint32 min() const
    {
        if (intervals.empty())
        {
            return 0;
        }
        return intervals.front().begin;
    }

    uint32 max() const
    {
        if (intervals.empty())
        {
            return 0;
        }
        return intervals.back().end;
    }

    std::pair<IntervalList, IntervalList> split(uint32 v) const
    {
        if (intervals.empty())
        {
            return {{}, {}};
        }
        if (v < min())
        {
            return {{}, *this};
        }
        if (v >= max())
        {
            return {*this, {}};
        }
        std::vector<Interval> left{};
        std::vector<Interval> right{};
        size_t index;
        for (index = 0; index < intervals.size(); ++index)
        {
            if (intervals[index].contains(v))
            {
                auto [l, r] = *intervals[index].split(v);
                left.push_back(l);
                right.push_back(r);
                ++index;
                break;
            }
            if (intervals[index].end > v)
            {
                break;
            }
            left.push_back(intervals[index]);
        }
        while (index < intervals.size())
        {
            right.push_back(intervals[index++]);
        }
        return {left, right};
    }

    uint64 n_elements() const
    {
        return ranges::accumulate(intervals | rv::transform(
                                                  [](const Interval& i)
                                                  {
                                                      return static_cast<uint64>(i.n_elements());
                                                  }),
                                  0,
                                  std::plus{});
    }

    auto find(uint32 v) const
    {
        return ranges::find_if(intervals,
                               [v](const Interval& i)
                               {
                                   return i.contains(v);
                               });
    }

    bool contains(uint32 v) const
    {
        return find(v) != intervals.end();
    }
};

struct PartRange
{
    IntervalList x{};
    IntervalList m{};
    IntervalList a{};
    IntervalList s{};

    std::pair<PartRange, PartRange> split(char prop, uint32 splitVal) const
    {
        if (prop == 'x')
        {
            auto [left, right] = x.split(splitVal);
            return std::make_pair(PartRange{left, m, a, s}, PartRange{right, m, a, s});
        }
        if (prop == 'm')
        {
            auto [left, right] = m.split(splitVal);
            return std::make_pair(PartRange{x, left, a, s}, PartRange{x, right, a, s});
        }
        if (prop == 'a')
        {
            auto [left, right] = a.split(splitVal);
            return std::make_pair(PartRange{x, m, left, s}, PartRange{x, m, right, s});
        }
        if (prop == 's')
        {
            auto [left, right] = s.split(splitVal);
            return std::make_pair(PartRange{x, m, a, left}, PartRange{x, m, a, right});
        }
        assert(false);
    }

    uint64 n_elements() const
    {
        return x.n_elements() * m.n_elements() * a.n_elements() * s.n_elements();
    }
};

// Workflows as trees
struct Leaf
{
    bool accepted;
};
struct Node;
using Tree = std::variant<Leaf, std::unique_ptr<Node>>;

struct Node
{
    Tree mLessThan, mGreaterThan;
    uint32 mSplitVal;
    char mProp;
};

template<typename... Ts>
struct overload: Ts...
{
    using Ts::operator()...;
};

uint64 countSuccess(const Tree& tree, const PartRange& partRange)
{
    return std::visit(
        overload{
            [&partRange](const Leaf& leaf) -> uint64
            {
                if (leaf.accepted)
                {
                    return partRange.n_elements();
                }
                return 0;
            },
            [&partRange](const std::unique_ptr<Node>& node) -> uint64
            {
                auto [left, right] = partRange.split(node->mProp, node->mSplitVal);
                return countSuccess(node->mLessThan, left) +
                       countSuccess(node->mGreaterThan, right);
            },
        },
        tree);
}

class TreeHolder
{
public:
    void addInstruction(const std::string& line)
    {
        auto split_line = split(line, "{");
        std::string label = split_line.front();
        std::string instruction = split_line.back();
        instruction.pop_back();
        mNodeInstructions[label] = instruction;
    }

    void buildTree()
    {
        mTree = buildTreeInternal("in");
    }

    uint64 part2() const
    {
        return countSuccess(mTree, PartRange{});
    }

private:
    Tree mTree;
    std::map<std::string, std::string> mNodeInstructions;

    Tree buildTreeInternal(const std::string& label)
    {
        if (label == "A")
        {
            return Leaf{true};
        }
        if (label == "R")
        {
            return Leaf{false};
        }
        auto it = mNodeInstructions.find(label);
        if (it == mNodeInstructions.end())
        {
            std::cerr << "Not found key: " << label << "\n";
        }
        const auto& instruction = it->second;

        return buildTreeFromInstruction(instruction);
    }

    Tree buildTreeFromInstruction(const std::string& instruction)
    {
        if (!instruction.contains(','))
        {
            return buildTreeInternal(instruction);
        }
        else
        {
            auto [command, restInstruction] = split_once(instruction, ",");
            // Parse command to know the compare value and if the command goes right or left
            assert(command.contains(':'));
            auto [condition, destinyLabel] = split_once(command, ":");
            if (condition.contains('<'))
            {
                auto [propStr, compareValue] = split_once(condition, "<");
                uint32 midVal = static_cast<uint32>(std::stoi(compareValue));
                assert(propStr.size() == 1);
                return std::unique_ptr<Node>(new Node{buildTreeInternal(destinyLabel),
                                                      buildTreeFromInstruction(restInstruction),
                                                      midVal,
                                                      propStr[0]});
            }
            else if (condition.contains('>'))
            {
                auto [propStr, compareValue] = split_once(condition, ">");
                uint32 midVal = static_cast<uint32>(std::stoi(compareValue)) + 1;
                assert(propStr.size() == 1);
                return std::unique_ptr<Node>(new Node{buildTreeFromInstruction(restInstruction),
                                                      buildTreeInternal(destinyLabel),
                                                      midVal,
                                                      propStr[0]});
            }
            else
            {
                assert(false);
            }
        }
    }
};

int main()
{
    std::ifstream file("input.txt");
    if (!file)
    {
        std::cerr << "Error opening file\n";
        return 1;
    }

    bool parseParts = false;
    std::string line;
    std::vector<Part> parts;
    std::map<std::string, Workflow> workflows;
    TreeHolder part2Tree;
    while (std::getline(file, line))
    {
        if (line.empty())
        {
            parseParts = true;
            continue;
        }
        if (parseParts)
        {
            std::vector<std::string> test = line | rv::drop(1) | rv::drop_last(1) | rv::split(',') |
                                            rg::to<std::vector<std::string>>();

            Part pa{};
            rg::for_each(test,
                         [&pa](auto&& m)
                         {
                             char type = m[0];
                             uint32 value =
                                 std::stoi(std::string(std::string_view(m.begin() + 2, m.end())));
                             switch (type)
                             {
                                 case 'x':
                                     pa.x = value;
                                     return;
                                 case 'm':
                                     pa.m = value;
                                     return;
                                 case 'a':
                                     pa.a = value;
                                     return;
                                 case 's':
                                     pa.s = value;
                                     return;
                             }
                         });
            parts.push_back(pa);
        }
        else
        {
            part2Tree.addInstruction(line);
            auto split_line = split(line, "{");
            std::string label = split_line.front();
            std::string rest = split_line.back();
            rest.pop_back();
            auto commands = split(rest, ",");
            workflows[label] = build_workflow(commands);
        }
    }
    PartsAnalyazer analyzer(workflows);
    uint32 totalPoints = 0;
    for (auto p: parts)
    {
        if (analyzer(p))
        {
            totalPoints += p.points();
        }
    }
    std::cout << "Part 1: " << totalPoints << "\n";

    part2Tree.buildTree();

    std::cout << "Part 2: " << part2Tree.part2() << "\n";
    return 0;
}
