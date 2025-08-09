#include <iostream>

#include <boost/algorithm/string/replace.hpp>

int main() {
    auto str = "12,345,456.00";
    auto z = boost::replace_all_copy<std::string>(str, ",", "");
    std::cout << str << std::endl;
    std::cout << z << std::endl;

    
}