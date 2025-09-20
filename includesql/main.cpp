// main.cpp
#include <iostream>
#include "generated_sql.h"  // This will be in CMAKE_BINARY_DIR

int main() {
    std::cout << "SQL Query: >>>>>>>>>>>>>>>>>>>>>\n"
              << SQL_QUERY << std::endl;
    std::cout << "<<<<<<<<<<<<<<<<<<<<<<<\n";
    // Use SQL_QUERY with your database library
    return 0;
}