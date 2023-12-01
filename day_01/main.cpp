#include <fstream>
#include <map>
#include <ranges>
#include <string>
#include <iostream>


const std::map<std::string, std::string> number_converter = {
    {"one", "1"},
    {"two", "2"},
    {"three", "3"},
    {"four", "4"},
    {"five", "5"},
    {"six", "6"},
    {"seven", "7"},
    {"eight", "8"},
    {"nine", "9"},
};


char get_first_digit(const std::ranges::view auto& line)
{
    auto digit = std::ranges::find_if(line, [](char c) { return std::isdigit(c); });
    if (digit != line.end()) {
        return *digit;
    }
    throw std::runtime_error("No digit found");
}

int get_number_part2(const std::string& line)
{
    // we only need to convert once from the beginning and once from the end
    std::string full_digit = "";
    std::pair<size_t, std::string> first_num_pos = {std::string::npos, ""};
    for (const auto& [word, digit] : number_converter)
    {
        auto pos = std::min(line.find(word), line.find(digit));
        if (pos != std::string::npos && pos < first_num_pos.first)
        {
            first_num_pos = {pos, digit};
        }
    }
    if (first_num_pos.first != std::string::npos)
    {
        full_digit += first_num_pos.second;
    }
    // now invert the string and do the same again
    std::pair<size_t, std::string> last_num_pos = {std::string::npos, ""};
    auto reversed_line = line;
    std::reverse(reversed_line.begin(), reversed_line.end());
    for (const auto& [word, digit] : number_converter)
    {
        auto reversed_word = word;
        std::reverse(reversed_word.begin(), reversed_word.end());
        auto pos = std::min(reversed_line.find(reversed_word), reversed_line.find(digit));
        if (pos != std::string::npos && pos < last_num_pos.first)
        {
            last_num_pos = {pos, digit};
        }
    }
    if (last_num_pos.first != std::string::npos)
    {
        full_digit += last_num_pos.second;
    }

    if (full_digit.empty())
    {
        std::cout << "No digits found in line: " << line << std::endl;
        return 0;
    }
    return std::stoi(full_digit);
}

int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    int part1 = 0;
    int part2 = 0;
    for (const auto& line : std::ranges::istream_view<std::string>(file)) {
        auto line_view = std::views::all(line);
        char first_digit = get_first_digit(line_view);
        char last_digit = get_first_digit(line_view | std::views::reverse);
        part1 += std::stoi(std::string({first_digit, last_digit}));
        part2 += get_number_part2(line);
    }
    std::cout << "Part 1: " << part1 << '\n';
    std::cout << "Part 2: " << part2 << '\n';

    return 0;
}
