#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

void test_main() {
    char choice {' '};
    int count {0};
    long input_num {0};
    std::cout << "Input number ";
    std::cin >> input_num;
    std::cout << "Input count ";
    std::cin >> choice;
    std::string s = std::to_string(input_num);
    std::cout << s << " is the string" << std::endl;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == choice)
            count++;
    }
    std::cout << std::endl
              << "counted " << count << std::endl;
}

int main() {
    std::string s = "test_is_coming_soon";

    std::cout << "no of _ in " << s << std::endl
              << std::count_if(s.begin(), s.end(), [](const char& e) { return e == '_'; }) << std::endl;
}
