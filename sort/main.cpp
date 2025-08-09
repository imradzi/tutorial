#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

auto sortList(std::vector<int>& v) -> void {
    std::vector<int> sorted(v.size());
    std::iota(sorted.begin(), sorted.end(), 0);
    std::sort(sorted.begin(), sorted.end(), [&v](int i1, int i2) {
        return v[i1] > v[i2];
    });
    for (auto i : sorted) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
} 

int main() {
  std::vector<int> v = {1, 2, 3, 4, 5, 10, 11, 12, 13, 14, 15}; 
  sortList(v);
}