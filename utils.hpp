/*
 * This file defines some utility functions that are useful for multiple days
 */

#include <string>
#include <iostream>
#include <ranges>
#include <vector>

std::vector<std::string> split(std::string s, const std::string& delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    size_t pos = 0;
    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);
    return tokens;
}

std::vector<std::string> split(std::string s)
{
    std::vector<std::string> tokens;
    std::string token;
    size_t pos = 0;
    while ((pos = s.find(" ")) != std::string::npos)
    {
        token = s.substr(0, pos);
        if (token != "")
        {
            tokens.push_back(token);
        }
        s.erase(0, pos + 1);
    }
    tokens.push_back(s);
    return tokens;
}

std::string strip(std::string s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
    return s;
}


template<std::ranges::range Container>
void print(const Container& container) {
    std::cout << '[';
    auto it = container.begin();
    if (it != container.end()) {
        std::cout << *it;
        ++it;
    }
    for (; it != container.end(); ++it) {
        std::cout << ", " << *it;
    }
    std::cout << ']' << std::endl;
}
