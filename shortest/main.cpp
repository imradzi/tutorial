#include <iostream>
#include <limits>
#include <vector>

struct Path {
    char from;
    char to;
    int cost;
};

std::vector<Path> net {
    { 'A', 'B', 3 },
    { 'A', 'C', 4 },
    { 'B', 'D', 1 },
    { 'B', 'F', 6 },
    { 'C', 'D', 2 },
    { 'C', 'E', 6 },
    { 'D', 'E', 4 },
    { 'D', 'F', 7 },
    { 'E', 'F', 3 },
    { 'E', 'G', 2 },
    { 'F', 'G', 5 }
};

std::vector<Path> findNext(char from) {
    std::vector<Path> result;
    for (const auto& x : net) {
        if (x.from == from) {
            result.push_back(x);
        }
    }
    return result;
}

int findShortest(char from, char to, std::vector<Path> &toTake) {
    int min = std::numeric_limits<int>::max();
    
    std::vector<Path> shortest;
    Path shortestNext;
    auto adj = findNext(from);

    for (auto const& x: adj) {
        std::vector<Path> tt;
        auto cost = x.cost + findShortest(x.to, to, tt);
        if (cost < min) {
            min = cost;
            shortest = tt;
            shortestNext = x;
        }
    }
    if (min < std::numeric_limits<int>::max()) {
        toTake.insert(toTake.begin(), shortest.begin(), shortest.end());
        toTake.insert(toTake.begin(), shortestNext);
        return min;
    }
    return 0;
}

int main() {
    std::vector<Path> shortest;
    auto cost = findShortest('A', 'G', shortest);
    for (auto const &x: shortest) {
        std::cout << x.from << "-> " << x.to << ": " << x.cost << std::endl;
    }
    std::cout << "Total cost " << cost << std::endl;
}