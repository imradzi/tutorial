#ifndef CSV_DATA_H
#define CSV_DATA_H

#include <string>

// Maximum sizes for arrays
const int MAX_POSTCODES = 60000;
const int MAX_STATES = 20;

// Structure to represent a postcode entry
struct PostcodeEntry {
    std::string postcode;
    std::string area;
    std::string city;
    std::string stateCode;
    
    // Default constructor
    PostcodeEntry() = default;
    
    // Constructor with parameters
    PostcodeEntry(const std::string& pc, const std::string& ar, 
                  const std::string& ci, const std::string& sc)
        : postcode(pc), area(ar), city(ci), stateCode(sc) {}
    
    // Print function for debugging
    void print() const;
};

// Structure to represent a state entry
struct StateEntry {
    std::string stateCode;
    std::string stateName;
    
    // Default constructor
    StateEntry() = default;
    
    // Constructor with parameters
    StateEntry(const std::string& code, const std::string& name)
        : stateCode(code), stateName(name) {}
    
    // Print function for debugging
    void print() const;
};

// Function declarations
std::string cleanString(const std::string& str);
std::vector<std::string> parseCSVLine(const std::string& line);
int readPostcodes(const std::string& filename, PostcodeEntry postcodes[], int maxSize);
int readStates(const std::string& filename, StateEntry states[], int maxSize);
std::string findStateName(const StateEntry states[], int stateCount, const std::string& stateCode);

#endif // CSV_DATA_H