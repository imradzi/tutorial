#include <algorithm>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <vector>

template <typename T>
void f(const T &, std::vector<T> &vect, typename std::vector<T>::iterator itbeg) {

}

auto main() -> int
{
    std::vector<int> v { 1, 2, 3, 4, 5, 6 };
    f(20, &v, v.begin());
}
