#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <iomanip>
#include "ULID.h"

// Mock MySQL prepared statement binding (for demonstration)
namespace mysql_demo {
    void bindBinary(const void* data, size_t size, const std::string& description) {
        std::cout << "Binding binary parameter (" << description << "): ";
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        for (size_t i = 0; i < size; ++i) {
            std::cout << std::hex << std::setfill('0') << std::setw(2) 
                      << static_cast<int>(bytes[i]);
        }
        std::cout << std::dec << std::endl;
    }
}

void demonstrateULIDBasics() {
    std::cout << "=== ULID Class Basics ===" << std::endl;
    
    // Generate new ULIDs
    ULID id1;  // Default constructor - generates new ULID
    ULID id2 = ULID::generate();  // Static method
    ULID id3 = ULID::generate(1234567890123ULL);  // With specific timestamp
    
    std::cout << "Generated ULIDs:" << std::endl;
    std::cout << "ID1: " << id1 << std::endl;  // Uses toString() automatically
    std::cout << "ID2: " << id2.toString() << std::endl;  // Explicit conversion
    std::cout << "ID3: " << id3 << " (custom timestamp)" << std::endl;
    
    // Create from string
    try {
        ULID id4("01FJYWZ3RJM927XKDJGDR06REA");  // From valid string
        std::cout << "ID4 from string: " << id4 << std::endl;
        
        // Extract timestamp
        std::cout << "ID4 timestamp: " << id4.timestamp() << " ms" << std::endl;
        
        // Convert to various formats
        std::cout << "ID4 hex: " << id4.toHex() << std::endl;
        std::cout << "ID4 binary size: " << id4.size() << " bytes" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    
    std::cout << std::endl;
}

void demonstrateDatabaseOperations() {
    std::cout << "=== Database Operations (Mock) ===" << std::endl;
    
    // Generate ULIDs for database records
    std::vector<ULID> record_ids;
    for (int i = 0; i < 3; ++i) {
        record_ids.emplace_back();  // Generate new ULID
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    // Simulate INSERT operations
    std::cout << "Simulating INSERT operations with binary ULIDs:" << std::endl;
    for (size_t i = 0; i < record_ids.size(); ++i) {
        const ULID& id = record_ids[i];
        
        std::cout << "\nINSERT INTO users (id, email) VALUES (?, ?);" << std::endl;
        std::cout << "String representation: " << id << std::endl;
        
        // This is what would be sent to MySQL (binary data)
        mysql_demo::bindBinary(id.data(), id.size(), "ULID primary key");
        std::cout << "Binding string: user" << (i+1) << "@example.com" << std::endl;
    }
    
    // Simulate SELECT with binary comparison
    std::cout << "\n--- SELECT Operations ---" << std::endl;
    std::cout << "SELECT * FROM users WHERE id > ? ORDER BY id;" << std::endl;
    
    ULID min_id = record_ids[1];  // Use second ID as minimum
    std::cout << "Minimum ID (string): " << min_id << std::endl;
    mysql_demo::bindBinary(min_id.data(), min_id.size(), "WHERE condition");
    
    std::cout << "\nResult set (sorted chronologically by binary comparison):" << std::endl;
    for (const auto& id : record_ids) {
        if (id > min_id) {
            std::cout << "Found record: " << id << " (timestamp: " 
                      << id.timestamp() << ")" << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void demonstrateBinaryComparison() {
    std::cout << "=== Binary Comparison Performance ===" << std::endl;
    
    // Generate test ULIDs
    const int TEST_COUNT = 1000;
    std::vector<ULID> ulids;
    ulids.reserve(TEST_COUNT);
    
    for (int i = 0; i < TEST_COUNT; ++i) {
        ulids.emplace_back();
    }
    
    // Test binary sorting performance
    auto start = std::chrono::high_resolution_clock::now();
    std::sort(ulids.begin(), ulids.end());  // Uses binary comparison
    auto end = std::chrono::high_resolution_clock::now();
    
    auto binary_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Sorted " << TEST_COUNT << " ULIDs using binary comparison in "
              << binary_duration.count() << " μs" << std::endl;
    
    // Verify sorting order
    bool is_sorted = std::is_sorted(ulids.begin(), ulids.end());
    std::cout << "Sorting result: " << (is_sorted ? "✅ Correctly sorted" : "❌ Not sorted") << std::endl;
    
    // Show first few sorted ULIDs
    std::cout << "First 5 sorted ULIDs:" << std::endl;
    for (int i = 0; i < 5 && i < TEST_COUNT; ++i) {
        std::cout << (i + 1) << ". " << ulids[i] 
                  << " (ts: " << ulids[i].timestamp() << ")" << std::endl;
    }
    
    std::cout << std::endl;
}

void demonstrateHashMapUsage() {
    std::cout << "=== HashMap Usage ===" << std::endl;
    
    // ULID can be used as key in unordered containers
    std::unordered_map<ULID, std::string> user_data;
    
    // Add some test data
    for (int i = 0; i < 5; ++i) {
        ULID id;
        std::string name = "User" + std::to_string(i + 1);
        user_data[id] = name;
        
        std::cout << "Added: " << id << " -> " << name << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    // Lookup by ULID
    std::cout << "\nLooking up users:" << std::endl;
    for (const auto& [id, name] : user_data) {
        std::cout << "Found: " << name << " with ID " << id << std::endl;
    }
    
    std::cout << std::endl;
}

int main() {
    std::cout << "ULID Class Demonstration" << std::endl;
    std::cout << "========================" << std::endl;
    
    demonstrateULIDBasics();
    demonstrateDatabaseOperations();
    demonstrateBinaryComparison();
    demonstrateHashMapUsage();
    
    std::cout << "=== Key Benefits of ULID Class ===" << std::endl;
    std::cout << "✅ Internal binary storage (16 bytes)" << std::endl;
    std::cout << "✅ String conversion only when needed" << std::endl;
    std::cout << "✅ Direct binary data for database operations" << std::endl;
    std::cout << "✅ Efficient comparison and sorting" << std::endl;
    std::cout << "✅ Type-safe operations" << std::endl;
    std::cout << "✅ STL container compatible" << std::endl;
    std::cout << "✅ Zero-copy database binding" << std::endl;
    
    return 0;
}