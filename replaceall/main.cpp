// main.cpp
#include <iostream>
#include <string>

std::string replace_all_sv(std::string str, std::string_view from, std::string_view to) {
    size_t start_pos = 0;
    
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    
    return str;
}

int main() {
    std::string x {"abc dd, 123 ddd"};
    auto res = replace_all_sv(x, "dd", "");
    std::cout << x << " => " << res << std::endl;
    res = replace_all_sv(x, "dd", "AxxxA");
    std::cout << x << " => " << res << std::endl;
    return 0;
}
