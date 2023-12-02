#include <fstream>
#include <ranges>
#include <string>
#include <iostream>


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
