// main.cpp
#include <iostream>
#include <string>
#include "bkk_ulid_functions.h"
#include "bkk_query2.h"
#include "bkk_query3.h"

int main() {
    std::cout << "=== bkk Database Queries ==="<< std::endl << std::endl;
    
    std::cout << "1. ULID Functions (" << sizeof(bkk_ULID_FUNCTIONS_QUERY) << " bytes):" << std::endl;
    std::cout << std::string(bkk_ULID_FUNCTIONS_QUERY).substr(0, 200) << "..." << std::endl << std::endl;
    
    std::cout << "2. User Management Query (" << sizeof(bkk_QUERY2) << " bytes):" << std::endl;
    std::cout << bkk_QUERY2 << std::endl << std::endl;
    
    std::cout << "3. Product Catalog Query (" << sizeof(bkk_QUERY3) << " bytes):" << std::endl;
    std::cout << bkk_QUERY3 << std::endl << std::endl;
    
    std::cout << "All SQL queries successfully embedded!" << std::endl;
    
    // Use the queries with your database library:
    // - bkk_ULID_FUNCTIONS_QUERY for ULID support
    // - bkk_QUERY2 for user management 
    // - bkk_QUERY3 for product catalog
    
    return 0;
}
