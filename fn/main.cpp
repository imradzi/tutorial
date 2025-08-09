#include <iostream>
// #include <functional>

bool asc(int a, int b) { return a < b; }

bool desc(int a, int b) { return b < a; }

int main() {
    bool (*funcPtr)(int a, int b);
    funcPtr = desc;
    // std::function<bool(int, int)> fptr;
    // fptr = asc;

    std::cout << "Result = " << funcPtr(20, 10) << std::endl;
    // std::cout << "Result = " << fptr(20, 10) << std::endl;
}
