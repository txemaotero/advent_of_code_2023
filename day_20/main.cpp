#include "../utils.hpp"

#include <array>
#include <deque>
#include <fstream>
#include <iostream>
#include <print>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

using signal_t = bool;
using namespace std::literals;

class Broadcaster;
class FlipFlop;
class Conjunction;

using ModuleType = std::variant<Broadcaster, FlipFlop, Conjunction>;

class Base
{
public:
    std::vector<std::string_view> targets;
};

class Broadcaster: public Base
{
public:
    static constexpr std::string_view name =  "broadcaster";

    std::optional<bool> processSignal(const std::string_view origin, const bool inSignal)
    {
        return inSignal;
    }
};

class FlipFlop: public Base
{
public:
    std::optional<bool> processSignal(const std::string_view origin, const bool inSignal)
    {
        if (inSignal)
        {
            return std::nullopt;
        }
        mState = !mState;
        return mState;
    }

private:
    bool mState {false};
};

class Conjunction: public Base
{
public:
    std::optional<bool> processSignal(const std::string_view origin, const bool inSignal)
    {
        inputStates[origin] = inSignal;
        return !std::ranges::all_of(inputStates,
                                    [](const auto& pair)
                                    {
                                        return pair.second;
                                    });
    }

    void addInput(const std::string_view in)
    {
        inputStates[in] = false;
    }

    const std::unordered_map<std::string_view, bool>& getInput() const
    {
        return inputStates;
    }

private:
    std::unordered_map<std::string_view, bool> inputStates;
};

struct Instruction
{
    std::string_view origin;
    bool pulse;
    std::string_view target;
};

class ButtonPressEvent
{
    static constexpr std::array parse = {"jg"sv, "rh"sv, "jm"sv, "hf"sv};

public:
    ButtonPressEvent(std::unordered_map<std::string, ModuleType>&& _modules):
        modules(std::move(_modules))
    {}

    bool press()
    {
        static uint64 nPresses = 0;
        instructions = {
            {"", false, Broadcaster::name}
        };

        while (!instructions.empty())
        {
            auto [origin, pulse, dest] = std::move(instructions.front());
            if (dest == "mg" && pulse)
            {
                if (part2Cycles.addOccurrence(*indexOf(parse, origin), nPresses))
                {
                    return true;
                }
            }
            instructions.pop_front();
            ++pulsesCount[pulse];
            auto it = modules.find(std::string(dest));
            if (it == modules.end())
            {
                continue;
            }
            auto& destModule = it->second;
            auto outSignal = std::visit(
                [&origin, &pulse](auto& mod) -> std::optional<bool>
                {
                    return mod.processSignal(origin, pulse);
                },
                destModule);
            if (!outSignal)
            {
                continue;
            }
            bool out = *outSignal;
            const std::vector<std::string_view>& newDests = std::visit(
                [](const auto& mod)
                {
                    return mod.targets;
                },
                destModule);
            for (const auto& newDest: newDests)
            {
                instructions.push_back({dest, out, newDest});
            }
        }
        ++nPresses;
        return false;
    }

    uint64 getCurrentResult() const
    {
        return pulsesCount[0] * pulsesCount[1];
    }

    std::vector<bool> getMGState() const
    {
        auto it = modules.find("mg");
        if (it == modules.end())
        {
            return {};
        }
        return std::visit(
            []<typename T>(const T& m)
            {
                if constexpr (std::is_same_v<T, Conjunction>)
                {
                    std::vector<bool> result;
                    for (const auto & [_, e]: m.getInput())
                    {
                        result.push_back(e);
                    }
                    return result;
                }
                else
                {
                    return std::vector<bool>{};
                }
            },
            it->second);
    }

    auto getPeriod()
    {
        return part2Cycles.getPeriod();
    }

private:
    std::unordered_map<std::string, ModuleType> modules;
    std::deque<Instruction> instructions;
    std::array<uint64, 2> pulsesCount {0u, 0u};
    ComposedCyclicProcess part2Cycles{4};
};


std::pair<std::string, ModuleType> moduleFactory(const std::string& definition)
{
    if (definition.starts_with('&'))
    {
        auto name = definition.substr(1);
        return {name, Conjunction()};
    }
    if (definition.starts_with('%'))
    {
        auto name = definition.substr(1);
        return {name, FlipFlop()};
    }
    return {std::string(Broadcaster::name), Broadcaster()};
}


int main()
{
    std::ifstream file("input.txt");
    if (!file)
    {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    std::unordered_map<std::string, ModuleType> modules;
    std::unordered_map<std::string, std::vector<std::string>> connections;
    while (std::getline(file, line))
    {
        auto [def, cons] = split_once(line, " -> ");
        auto [name, module] = moduleFactory(def);
        modules[name] = module;
        connections[name] = split(cons, ", ");
    }
    for (const auto& [name, targets]: connections)
    {
        auto& refMod = modules[name];
        for (const auto& targetName: targets)
        {
            std::visit(
                [&targetName](auto& ref)
                {
                    ref.targets.push_back(targetName);
                },
                refMod);
            auto& tarMod = modules[targetName];
            std::visit(
                [&name]<typename T>(T& tar)
                {
                    if constexpr (std::is_same_v<T, Conjunction>)
                    {
                        tar.addInput(name);
                    }
                },
                tarMod);
        }
    }

    ButtonPressEvent game(std::move(modules));
    uint64 gameTurn = 0;
    while (true)
    {
        if (game.press())
        {
            std::println("Part 2: {}", *game.getPeriod());
            break;
        }

        if (++gameTurn == 1000)
        {
            std::println("Part 1: {}", game.getCurrentResult());
        }
    }

    return 0;
}
