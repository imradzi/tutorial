#include <algorithm>
#include <chrono>
#include <random>
#include <iostream>

struct Random {
    std::random_device re;
    std::uniform_int_distribution<> dist;
    std::mt19937 gen;

//    Random(int low, int high): dist(low, high), gen(re()){}
    Random(int low, int high) : dist(low, high) {}
    auto operator()() { return dist(re); }
};

int main() {
    Random randomizer(20, 100);
    for (int i = 0; i< 20; i++) {
        std::cout << randomizer() << ",";
    }
    std::cout << std::endl;
}