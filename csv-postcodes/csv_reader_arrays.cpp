#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "csv_data.h"

// Implementation of print functions
void PostcodeEntry::print() const {
    std::cout << "Postcode: " << postcode << ", Area: " << area 
              << ", City: " << city << ", State: " << stateCode << std::endl;
}

void StateEntry::print() const {
    std::cout << "State Code: " << stateCode << ", State Name: " << stateName << std::endl;
}

// Function to remove quotes and trim whitespace from a string
std::string cleanString(const std::string& str) {
    std::string result = str;
    
    // Remove leading and trailing whitespace
    size_t start = result.find_first_not_of(" \t\r\n");
    if (start != std::string::npos) {
        result = result.substr(start);
    }
    size_t end = result.find_last_not_of(" \t\r\n");
    if (end != std::string::npos) {
        result = result.substr(0, end + 1);
    }
    
    // Remove quotes if they exist
    if (result.length() >= 2 && result.front() == '"' && result.back() == '"') {
        result = result.substr(1, result.length() - 2);
    }
    
    return result;
}

// Function to parse CSV line with quoted fields
std::vector<std::string> parseCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;
    
    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(cleanString(field));
            field.clear();
        } else {
            field += c;
        }
    }
    
    // Add the last field
    if (!field.empty() || line.back() == ',') {
        fields.push_back(cleanString(field));
    }
    
    return fields;
}

// Function to read postcodes from CSV file into array
int readPostcodes(const std::string& filename, PostcodeEntry postcodes[], int maxSize) {
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return 0;
    }
    
    std::string line;
    int count = 0;
    int lineNumber = 0;
    
    while (std::getline(file, line) && count < maxSize) {
        lineNumber++;
        
        if (line.empty()) {
            continue;
        }
        
        std::vector<std::string> fields = parseCSVLine(line);
        
        if (fields.size() >= 4) {
            postcodes[count] = PostcodeEntry(fields[0], fields[1], fields[2], fields[3]);
            count++;
        } else {
            std::cerr << "Warning: Insufficient fields at line " << lineNumber 
                     << " (found " << fields.size() << " fields)" << std::endl;
        }
    }
    
    file.close();
    std::cout << "Successfully loaded " << count << " postcode entries." << std::endl;
    return count;
}

// Function to read states from CSV file into array
int readStates(const std::string& filename, StateEntry states[], int maxSize) {
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return 0;
    }
    
    std::string line;
    int count = 0;
    int lineNumber = 0;
    
    while (std::getline(file, line) && count < maxSize) {
        lineNumber++;
        
        if (line.empty()) {
            continue;
        }
        
        std::vector<std::string> fields = parseCSVLine(line);
        
        if (fields.size() >= 2) {
            states[count] = StateEntry(fields[0], fields[1]);
            count++;
        } else {
            std::cerr << "Warning: Insufficient fields at line " << lineNumber 
                     << " (found " << fields.size() << " fields)" << std::endl;
        }
    }
    
    file.close();
    std::cout << "Successfully loaded " << count << " state entries." << std::endl;
    return count;
}

// Function to find state name by state code using arrays
std::string findStateName(const StateEntry states[], int stateCount, const std::string& stateCode) {
    for (int i = 0; i < stateCount; ++i) {
        if (states[i].stateCode == stateCode) {
            return states[i].stateName;
        }
    }
    return "Unknown State";
}

int main() {
    // Declare arrays to hold data
    PostcodeEntry* postcodes = new PostcodeEntry[MAX_POSTCODES];
    StateEntry* states = new StateEntry[MAX_STATES];
    
    // Read data into arrays
    int postcodeCount = readPostcodes("postcodes.csv", postcodes, MAX_POSTCODES);
    int stateCount = readStates("states.csv", states, MAX_STATES);
    
    if (postcodeCount == 0) {
        std::cerr << "No postcode data loaded." << std::endl;
        delete[] postcodes;
        delete[] states;
        return 1;
    }
    
    if (stateCount == 0) {
        std::cerr << "No state data loaded." << std::endl;
        delete[] postcodes;
        delete[] states;
        return 1;
    }
    
    std::cout << "\n=== Data loaded successfully ===" << std::endl;
    std::cout << "Total postcodes: " << postcodeCount << std::endl;
    std::cout << "Total states: " << stateCount << std::endl;
    
    // Display first 5 postcodes as example
    std::cout << "\n=== First 5 Postcode Entries ===" << std::endl;
    for (int i = 0; i < std::min(postcodeCount, 5); ++i) {
        std::cout << i + 1 << ". ";
        postcodes[i].print();
    }
    
    // Display all states
    std::cout << "\n=== All States ===" << std::endl;
    for (int i = 0; i < stateCount; ++i) {
        std::cout << i + 1 << ". ";
        states[i].print();
    }
    
    // Example usage: Find postcodes for a specific state
    std::cout << "\n=== Sample Postcodes for Perlis (PLS) ===" << std::endl;
    int count = 0;
    for (int i = 0; i < postcodeCount && count < 10; ++i) {
        if (postcodes[i].stateCode == "PLS") {
            std::cout << postcodes[i].area << " (" << postcodes[i].postcode << ") - " 
                     << findStateName(states, stateCount, postcodes[i].stateCode) << std::endl;
            count++;
        }
    }
    
    // Interactive search example
    std::cout << "\n=== Interactive Search ===" << std::endl;
    std::string searchCode;
    std::cout << "Enter state code to search (e.g., PLS, JHR, KUL): ";
    std::cin >> searchCode;
    
    std::cout << "Areas in state " << searchCode << ":" << std::endl;
    count = 0;
    for (int i = 0; i < postcodeCount && count < 20; ++i) {
        if (postcodes[i].stateCode == searchCode) {
            std::cout << count + 1 << ". " << postcodes[i].area 
                     << " (" << postcodes[i].postcode << ")" << std::endl;
            count++;
        }
    }
    
    if (count == 0) {
        std::cout << "No postcodes found for state code: " << searchCode << std::endl;
    } else {
        std::cout << "\nShowing " << count << " results (limited to 20)." << std::endl;
        
        // Count total results for this state
        int total = 0;
        for (int i = 0; i < postcodeCount; ++i) {
            if (postcodes[i].stateCode == searchCode) {
                total++;
            }
        }
        std::cout << "Total postcodes found for " << searchCode << ": " << total << std::endl;
    }
    
    // Clean up memory
    delete[] postcodes;
    delete[] states;
    
    return 0;
}