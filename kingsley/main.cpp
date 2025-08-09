#include <iostream>
#include <ctime>

template<typename T>
T templ_print(T x) {
    return x * x;
}

int func_print(int x) {
    return x * x;
}
#define PRINT(x) x* x
void res_macro() {
    int x = 2;
    std::cout << "Using macro" << std::endl;
    std::cout << PRINT(x++) << std::endl;
    std::cout << x << std::endl;
}

void res_func() {
    int x = 2;
    std::cout << "Using function" << std::endl;
    std::cout << func_print(x++) << std::endl;
    std::cout << x << std::endl;
}

void res_templ() {
    int x = 2;
    std::cout << "Using template" << std::endl;
    std::cout << templ_print(x++) << std::endl;
    std::cout << x << std::endl;
}

int xmain() {
    res_macro();
    res_func();
    res_templ();
    return 0;
}

int main() {
    int x = 10, count = 0;
    auto timer = std::clock();
    while (x > 0) {
        if (std::clock() > (timer + 1000000)) {
            timer = std::clock();
            std::cout << x << "\t" << count << std::endl;
            count = 0;
            x--;
        }
        count++;
    }
    return 0;
}
