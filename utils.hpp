/*
 * This file defines some utility functions that are useful for multiple days
 */

#include <concepts>
#include <string>
#include <iostream>
#include <ranges>
#include <vector>


using uint32 = unsigned int;
using uint64 = unsigned long long int;
using int32 = int;
using int64 = long long int;



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

template<typename T, typename F>
concept Comparator = requires(F f, const T& a, const T& b) {
    { f(a, b) } -> std::convertible_to<bool>;
};

template<typename T, typename F = std::less<T>>
    requires Comparator<T, F>
class PriorityQueue {
public:
    PriorityQueue() = default;

    void push(const T& item) {
        auto it = std::lower_bound(data.begin(), data.end(), item, comparator);
        data.insert(it, item);
    }

    T pop() {
        if (data.empty()) {
            throw std::runtime_error("PriorityQueue is empty");
        }
        T result = data[0];
        data.erase(data.begin());
        return result;
    }

    T popLast() {
        T result = data.back();
        data.pop_back();
        return result;
    }

    T first() const {
        return data[0];
    }

    T last() const {
        return data.back();
    }

    bool empty() const {
        return data.empty();
    }

    size_t size() const {
        return data.size();
    }

    const std::vector<T>& getData() const {
        return data;
    }

    std::vector<T>& getData() {
        return data;
    }

private:
    std::vector<T> data;
    F comparator;
};
