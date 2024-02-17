#include <fstream>
#include <functional>
#include <string>
#include <iostream>
#include <range/v3/all.hpp>
#include <string_view>
#include "../utils.hpp"

namespace rv = ranges::views;
namespace rg = ranges;

struct Part
{
    uint32 x;
    uint32 m;
    uint32 a;
    uint32 s;
    bool rejected {false};
    bool accepted {false};
};


using Workflow = std::function<std::string(const Part&)>;

int main() {
    // std::ifstream file("input.txt");
    std::ifstream file("example.txt");
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    bool parseParts = false;
    std::string line;
    std::vector<Part> parts;
    std::map<std::string, Workflow> workflows;
    while (std::getline(file, line)) {
        if (line.empty())
        {
            parseParts = true;
            continue;
        }
        if (parseParts)
        {
            std::vector<std::string> test = line
                | rv::drop(1)
                | rv::drop_last(1)
                | rv::split(',')
                | rg::to<std::vector<std::string>>();

            Part pa {};
            rg::for_each(test, [&pa](auto&& m){
                    char type = m[0];
                    uint32 value = std::stoi(std::string(std::string_view(m.begin() + 2, m.end())));
                    switch (type) {
                        case 'x': pa.x = value; return;
                        case 'm': pa.m = value; return;
                        case 'a': pa.a = value; return;
                        case 's': pa.s = value; return;
                    }
                    });
            parts.push_back(pa);
        }
        else
        {

        }
    }
    for (auto p : parts)
    {
        std::cout << p.x << " " << p.m << " " << p.a << " " << p.s << std::endl;
    }

    return 0;
}
