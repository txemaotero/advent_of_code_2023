#include "../utils.hpp"

#include <deque>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>


using signal_t = bool;

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
public:
    ButtonPressEvent(std::unordered_map<std::string, ModuleType>&& _modules):
        modules(std::move(_modules))
    {}

    bool press()
    {
        bool lowPulseToRx = false;
        instructions = {
            {"", false, Broadcaster::name}
        };

        while (!instructions.empty())
        {
            // std::cout << "Processing Instruction\n";
            auto [origin, pulse, dest] = std::move(instructions.front());
            instructions.pop_front();
            // std::cout << "pulse count: " << pulsesCount[0] << ", " << pulsesCount[1] << "\n";
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
                // std::cout << "Adding Instruction\n";
                instructions.push_back({dest, out, newDest});
                if (newDest == "rx" && !out)
                {
                    lowPulseToRx = true;
                }
                // if (newDest == "mg" && out)
                // {
                //     std::cout << "Aqui desde " << dest << "\n";
                // }
            }
        }
        return lowPulseToRx;
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

private:
    std::unordered_map<std::string, ModuleType> modules;
    std::deque<Instruction> instructions;
    std::array<uint64, 2> pulsesCount {0u, 0u};
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

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

std::string toStr(const std::vector<bool>& v)
{
    if (v.empty())
    {
        return "";
    }
    std::string result = std::to_string(static_cast<int>(v[0]));
    for (int i = 1; i < v.size(); ++i)
    {
        result += " " + std::to_string(static_cast<int>(v[0]));
    }
    return result;
}

int main()
{
    std::ifstream file("input.txt");
    // std::ifstream file("example2.txt");
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
        // std::cout << "Module: " << name <<" -> " << "cons: " << connections[name].size() << "\n";
    }
    // std::cout << modules.size() << " modules\n";
    // std::cout << connections.size() << " connections\n";
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
    bool endPart2 = false;
    std::vector<bool> mgState;
    for (uint64 n = 0; n < 1000 || !endPart2; ++n)
    {
        endPart2 = game.press();
        if (endPart2)
        {
            std::cout << "Part 2: " << n + 1 << "\n";
        }
        if (auto newState = game.getMGState(); mgState != newState)
        {
            std::cout << "New state " << toStr(newState) << " at n = " << n << "\n";
            mgState = newState;
        }
    }
    std::cout << "Part 1: " << game.getCurrentResult() << "\n";

    return 0;
}
