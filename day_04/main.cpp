#include <algorithm>
#include <fstream>
#include <numeric>
#include <ranges>
#include <string>
#include <iostream>
#include "../utils.hpp"


class Game {
public:
    Game(const std::string& line)
    {
        auto lineSpl = split(line, ": ");
        gameId = std::stoi(split(lineSpl[0], " ")[1]);
        auto numsPart = split(lineSpl[1], " | ");
        auto winningNumsVec = split(numsPart[0], " ");
        auto playingNumsVec = split(numsPart[1], " ");
        for (auto& num : winningNumsVec) {
            winningNums.push_back(std::stoi(num));
        }
        for (auto& num : playingNumsVec) {
            playingNums.push_back(std::stoi(num));
        }
        std::ranges::sort(winningNums);
    }

    int getGameId() const { return gameId; }

    int getNumWins() const
    {
        return std::count_if(playingNums.begin(), playingNums.end(), [this](int num) {
            return std::binary_search(winningNums.begin(), winningNums.end(), num);
        });
    }

    int score() const
    {
        int wins = getNumWins();

        if (wins == 0) {
            return 0;
        }
        return std::pow(2, wins - 1);
    }

private:
    int gameId;
    std::vector<int> winningNums;
    std::vector<int> playingNums;

};

class Maze {
public:
    Maze() = default;

    void addGame(const Game& game)
    {
        games.push_back(game);
        winningNums.push_back(game.getNumWins());
        generatedCards.push_back(0);
    }

    void computeGeneratedCards()
    {
        // We know the last generates 0
        const int numGames = games.size();
        generatedCards[numGames - 1] = 1;
        for (int i : std::views::iota(0, numGames - 1) | std::views::reverse) {
            const auto& game = games[i];
            const int numWins = winningNums[i];
            int generatedCard = 0;
            for (int j : std::views::iota(1, numWins + 1)) {
                generatedCard += generatedCards[i + j];
            }
            generatedCards[i] = generatedCard + 1;
        }
    }

    int getNumberOfScratchCards()
    {
        computeGeneratedCards();
        return std::accumulate(generatedCards.begin(), generatedCards.end(), 0);
    }


private:
    std::vector<Game> games;
    std::vector<int> winningNums;
    std::vector<int> generatedCards;
};

int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::string line;
    int part1 = 0;
    Maze maze {};
    while (std::getline(file, line)) {
        Game game(line);
        part1 += game.score();
        maze.addGame(game);
    }

    std::cout << "Part 1: " << part1 << "\n";
    std::cout << "Part 2: " << maze.getNumberOfScratchCards() << "\n";
    return 0;
}
