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

    for (const auto& line : std::ranges::istream_view<std::string>(file)) {
        // Do something with line
        std::cout << line << '\n';
    }

    return 0;
}
