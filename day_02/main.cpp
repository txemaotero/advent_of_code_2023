#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <algorithm>
#include "../utils.hpp"


class Turn {
public:
    int red {0};
    int green {0};
    int blue {0};

    Turn(const std::string& turnStr) {
        auto colorsSpl = split(turnStr, ", ");
        for (const auto& colorStr : colorsSpl) {
            setColor(colorStr);
        }
    }

    void setColor(const std::string& colorStr) {
        auto colorSpl = split(colorStr, " ");
        int colorNum = std::stoi(colorSpl[0]);
        std::string colorName = strip(colorSpl[1]);
        if (colorName == "red") {
            red = colorNum;
        } else if (colorName == "green") {
            green = colorNum;
        } else if (colorName == "blue") {
            blue = colorNum;
        } else {
            std::cerr << "Invalid color name: " << colorName << '\n';
        }
    }

};


class Game {
public:
    static constexpr int RED_LIMIT = 12;
    static constexpr int GREEN_LIMIT = 13;
    static constexpr int BLUE_LIMIT = 14;

    Game(const std::string& line)
    {
        auto lineSpl = split(line, ": ");
        id = std::stoi(split(lineSpl[0], " ")[1]);
        auto turnsStrings = split(lineSpl[1], "; ");
        for (const auto& turnStr : turnsStrings) {
            turns.emplace_back(turnStr);
        }
    }

    bool isValid() const {
        return std::ranges::all_of(turns, [](const auto& turn) {
            return turn.red <= RED_LIMIT && turn.green <= GREEN_LIMIT && turn.blue <= BLUE_LIMIT;
        });
    }

    int getId() const {
        return id;
    }

    std::array<int, 3> getMinBallNumbers() const {
        std::array<int, 3> result {0, 0, 0};
        for (const auto& turn : turns) {
            result[0] = std::max(result[0], turn.red);
            result[1] = std::max(result[1], turn.green);
            result[2] = std::max(result[2], turn.blue);
        }
        return result;
    }

private:
    std::vector<Turn> turns;
    int id;
};



int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    int resultPart1 = 0;
    int resultPart2 = 0;
    while (std::getline(file, line)) {
        Game game(line);
        if (game.isValid()) {
            resultPart1 += game.getId();
        }
        auto minBallNumbers = game.getMinBallNumbers();
        resultPart2 += minBallNumbers[0] * minBallNumbers[1] * minBallNumbers[2];
    }

    std::cout << "Part 1: " << resultPart1 << '\n';
    std::cout << "Part 2: " << resultPart2 << '\n';

    return 0;
}
