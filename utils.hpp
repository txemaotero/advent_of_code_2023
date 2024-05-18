/*
 * This file defines some utility functions that are useful for multiple days
 */

#include <algorithm>
#include <cassert>
#include <concepts>
#include <numeric>
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

std::pair<std::string, std::string> split_once(std::string s, const std::string& delimiter)
{
    std::pair<std::string, std::string> result;
    size_t pos = s.find(delimiter);
    if (pos == std::string::npos)
    {
        throw std::runtime_error("Delimiter not found");
    }
    result.first = s.substr(0, pos);
    s.erase(0, pos + delimiter.length());
    result.second = s;
    return result;
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

template<std::ranges::range Container>
std::optional<size_t> indexOf(const Container& container, const typename Container::value_type& element) {
    if (auto it = std::ranges::find(container, element); it == std::end(container))
    {
        return {};
    }
    else
    {
        return std::distance(std::begin(container), it);
    }
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

class CyclicProcess
{
public:
    bool addOccurrence(uint64 step)
    {
        occurrences.push_back(step);
        return isCyclic();
    }

    bool isCyclic() const
    {
        auto s = occurrences.size();
        auto period1 = occurrences[s - 1] - occurrences[s - 2];
        auto period2 = occurrences[s - 2] - occurrences[s - 3];
        return s > 1 && period1 == period2;
    }

    std::optional<uint64> getPeriod() const
    {
        if (!isCyclic())
        {
            return std::nullopt;
        }
        auto s = occurrences.size();
        return occurrences[s - 1] - occurrences[s - 2];
    }

    const std::vector<uint64>& getOccurrences() const
    {
        return occurrences;
    }

private:
    std::vector<uint64> occurrences {};
};

class ComposedCyclicProcess
{
public:
    ComposedCyclicProcess(size_t nProccess) : cycles(nProccess) {}

    bool addOccurrence(size_t processId, uint64 step)
    {
        assert(processId < cycles.size());
        cycles[processId].addOccurrence(step);
        return allHaveCycles();
    }

    bool allHaveCycles() const
    {
        return std::ranges::all_of(cycles, &CyclicProcess::isCyclic);
    }

    std::optional<uint64> getPeriod() const
    {
        if (!allHaveCycles())
        {
            return std::nullopt;
        }
        const auto& periods = cycles | std::views::transform(
                                           [](const auto& c) -> uint64
                                           {
                                               auto p = c.getPeriod();
                                               return p ? *p : 0;
                                           });
        return std::ranges::fold_left(periods,
                                      1u,
                                      [](uint64 acc, uint64 p)
                                      {
                                          return std::lcm(acc, p);
                                      });
    }

private:
    std::vector<CyclicProcess> cycles;
};

