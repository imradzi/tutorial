#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

// Structure to represent a postcode entry
struct PostcodeEntry {
    std::string postcode;
    std::string area;
    std::string city;
    std::string stateCode;
    
    // Constructor
    PostcodeEntry(const std::string& pc = "", const std::string& ar = "", 
                  const std::string& ci = "", const std::string& sc = "")
        : postcode(pc), area(ar), city(ci), stateCode(sc) {}
    
    // Print function for debugging
    void print() const {
        std::cout << "Postcode: " << postcode << ", Area: " << area 
                  << ", City: " << city << ", State: " << stateCode << std::endl;
    }
};

// Structure to represent a state entry
struct StateEntry {
    std::string stateCode;
    std::string stateName;
    
    // Constructor
    StateEntry(const std::string& code = "", const std::string& name = "")
        : stateCode(code), stateName(name) {}
    
    // Print function for debugging
    void print() const {
        std::cout << "State Code: " << stateCode << ", State Name: " << stateName << std::endl;
    }
};

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

// Function to read postcodes from CSV file
std::vector<PostcodeEntry> readPostcodes(const std::string& filename) {
    std::vector<PostcodeEntry> postcodes;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return postcodes;
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        if (line.empty()) {
            continue;
        }
        
        std::vector<std::string> fields = parseCSVLine(line);
        
        if (fields.size() >= 4) {
            postcodes.emplace_back(fields[0], fields[1], fields[2], fields[3]);
        } else {
            std::cerr << "Warning: Insufficient fields at line " << lineNumber 
                     << " (found " << fields.size() << " fields)" << std::endl;
        }
    }
    
    file.close();
    std::cout << "Successfully loaded " << postcodes.size() << " postcode entries." << std::endl;
    return postcodes;
}

// Function to read states from CSV file
std::vector<StateEntry> readStates(const std::string& filename) {
    std::vector<StateEntry> states;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return states;
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        if (line.empty()) {
            continue;
        }
        
        std::vector<std::string> fields = parseCSVLine(line);
        
        if (fields.size() >= 2) {
            states.emplace_back(fields[0], fields[1]);
        } else {
            std::cerr << "Warning: Insufficient fields at line " << lineNumber 
                     << " (found " << fields.size() << " fields)" << std::endl;
        }
    }
    
    file.close();
    std::cout << "Successfully loaded " << states.size() << " state entries." << std::endl;
    return states;
}

// Function to find state name by state code
std::string findStateName(const std::vector<StateEntry>& states, const std::string& stateCode) {
    for (const auto& state : states) {
        if (state.stateCode == stateCode) {
            return state.stateName;
        }
    }
    return "Unknown State";
}

int main() {
    // Read postcodes and states into arrays (vectors)
    std::vector<PostcodeEntry> postcodes = readPostcodes("postcodes.csv");
    std::vector<StateEntry> states = readStates("states.csv");
    
    if (postcodes.empty()) {
        std::cerr << "No postcode data loaded." << std::endl;
        return 1;
    }
    
    if (states.empty()) {
        std::cerr << "No state data loaded." << std::endl;
        return 1;
    }
    
    std::cout << "\n=== Data loaded successfully ===" << std::endl;
    std::cout << "Total postcodes: " << postcodes.size() << std::endl;
    std::cout << "Total states: " << states.size() << std::endl;
    
    // Display first 5 postcodes as example
    std::cout << "\n=== First 5 Postcode Entries ===" << std::endl;
    for (size_t i = 0; i < std::min(postcodes.size(), size_t(5)); ++i) {
        std::cout << i + 1 << ". ";
        postcodes[i].print();
    }
    
    // Display all states
    std::cout << "\n=== All States ===" << std::endl;
    for (size_t i = 0; i < states.size(); ++i) {
        std::cout << i + 1 << ". ";
        states[i].print();
    }
    
    // Example usage: Find postcodes for a specific state
    std::cout << "\n=== Sample Postcodes for Perlis (PLS) ===" << std::endl;
    int count = 0;
    for (const auto& postcode : postcodes) {
        if (postcode.stateCode == "PLS" && count < 10) {  // Show only first 10
            std::cout << postcode.area << " (" << postcode.postcode << ") - " 
                     << findStateName(states, postcode.stateCode) << std::endl;
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
    for (const auto& postcode : postcodes) {
        if (postcode.stateCode == searchCode && count < 20) {  // Limit to 20 results
            std::cout << count + 1 << ". " << postcode.area << " (" << postcode.postcode << ")" << std::endl;
            count++;
        }
    }
    
    if (count == 0) {
        std::cout << "No postcodes found for state code: " << searchCode << std::endl;
    } else {
        std::cout << "\nShowing " << count << " results (limited to 20)." << std::endl;
        
        // Count total results for this state
        int total = 0;
        for (const auto& postcode : postcodes) {
            if (postcode.stateCode == searchCode) {
                total++;
            }
        }
        std::cout << "Total postcodes found for " << searchCode << ": " << total << std::endl;
    }
    
    return 0;
}