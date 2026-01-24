#include <iostream>
#include <iomanip>

int main() {
    try {
        double f = 4.34;
        std::cout << std::setprecision(10) << f << std::endl;
    } catch (std::exception &e) {
        std::cout << "\nException : " << e.what() << std::endl;
    }
}
