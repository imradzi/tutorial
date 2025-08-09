#include <algorithm>
#include <vector>
#include <iostream>

struct S {
    int number;
    char name;
    // note: name is ignored by this comparison operator
    bool operator<(const S& s) const { return number < s.number; }
};

int main() {
    // note: not ordered, only partitioned w.r.t. S defined below
    const std::vector<S> vec = {{2, 'A'}, {4, 'A'}, {2, 'B'}, {2, 'C'}, {2, 'D'}, {2, 'G'}, {3, 'F'}, {2, 'F'}};

    const S value = {2, '?'};

    std::cout << "\nCompare using S::operator<(): ";
    const auto p = std::equal_range(vec.begin(), vec.end(), value);

    for (auto i = p.first; i != p.second; ++i) {
        std::cout << i->name << ' ';
    }
    std::cout << std::endl;
}