#include <fmt/format.h>
#include <nlohmann/json.hpp>
int main() {
    try {
    } catch (std::exception &e) {
        std::cout << "\nException : " << e.what() << std::endl;
    }
}
