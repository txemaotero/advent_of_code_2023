#include "../utils.hpp"

#include <range/v3/all.hpp>

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>

namespace rv = ranges::views;
namespace rg = ranges;

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

    std::pair<IntervalList, IntervalList> split(uint32 v)
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

    uint32 n_elements() const
    {
        return ranges::accumulate(intervals | rv::transform(
                                                  [](const Interval& i)
                                                  {
                                                      return i.n_elements();
                                                  }),
                                  0);
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

int main()
{
    std::ifstream file("input.txt");
    // std::ifstream file("example.txt");
    if (!file)
    {
        std::cerr << "Error opening file\n";
        return 1;
    }

    bool parseParts = false;
    std::string line;
    std::vector<Part> parts;
    std::map<std::string, Workflow> workflows;
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
    return 0;
}
