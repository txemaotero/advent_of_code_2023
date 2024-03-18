#include <deque>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <iostream>
#include <variant>
#include <vector>
#include "../utils.hpp"


typedef bool signal_t;

class IModule
{
public:
    virtual std::vector<std::pair<IModule*, signal_t>> processSignal(signal_t inputSignal) = 0;
    virtual std::string getName() const = 0;
};

typedef std::vector<std::unique_ptr<IModule>> ModulePool;
typedef std::deque<std::pair<IModule*, signal_t>> InstructionsQueue;

class ButtonPressEvent
{
public:
    void press()
    {
        instructions = {std::make_pair(broadcasterPtr, false)};
        while (!instructions.empty())
        {
            auto [module, sign] = std::move(instructions.front());
            instructions.pop_front();
            if (sign)
            {
                ++numOfHighPulses;
            }
            else
            {
                ++numOfLowPulses;
            }
            for (const auto outInstructions = module->processSignal(sign); auto& outInstruction : outInstructions)
            {
                instructions.push_back(outInstruction);
            }
        }
    }

    uint64 getCurrentResult() const
    {
        return numOfLowPulses * numOfHighPulses;
    }

private:
    ModulePool modules;
    IModule* broadcasterPtr;
    InstructionsQueue instructions;
    uint64 numOfLowPulses {0u};
    uint64 numOfHighPulses {0u};
};

class Broadcaster : public IModule
{
public:
    std::vector<std::pair<IModule *, signal_t>> processSignal(signal_t inputSignal) override
    {
        return {};
    }

    std::string getName() const override
    {
        return "broadcaster";
    }
};

class FlipFlop : public IModule
{
public:
    std::vector<std::pair<IModule *, signal_t>> processSignal(signal_t inputSignal) override
    {
        return {};
    }

    std::string getName() const override
    {
        return "a";
    }
};

class Conjunction : public IModule
{
public:
    std::vector<std::pair<IModule *, signal_t>> processSignal(signal_t inputSignal) override
    {
        return {};
    }

    std::string getName() const override
    {
        return "b";
    }
};


typedef std::variant<Broadcaster, FlipFlop, Conjunction> ModuleType;


ModuleType moduleFactory(const std::string& definition)
{
    return Broadcaster();
}

int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << '\n';
    }

    return 0;
}
