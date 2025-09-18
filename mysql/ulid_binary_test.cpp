#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>
#include <cstring>
#include <sstream>
#include <thread>
#include "ulid.h"

// Convert ULID string to 16-byte binary representation
std::array<uint8_t, 16> ulidStringToBinary(const std::string& ulid) {
    if (ulid.length() != 26) {
        throw std::invalid_argument("Invalid ULID length");
    }
    
    std::array<uint8_t, 16> binary = {};
    
    // Crockford's Base32 decoding table
    const int8_t DECODING[] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
        -1, 10, 11, 12, 13, 14, 15, 16, 17, -1, 18, 19, -1, 20, 21, -1,
        22, 23, 24, 25, 26, -1, 27, 28, 29, 30, 31, -1, -1, -1, -1, -1,
        -1, 10, 11, 12, 13, 14, 15, 16, 17, -1, 18, 19, -1, 20, 21, -1,
        22, 23, 24, 25, 26, -1, 27, 28, 29, 30, 31, -1, -1, -1, -1, -1
    };
    
    // Decode ULID string to binary
    int bit_count = 0;
    uint64_t accumulator = 0;
    int byte_index = 0;
    
    for (char c : ulid) {
        if (c < 0 || c >= 128 || DECODING[c] == -1) {
            throw std::invalid_argument("Invalid character in ULID");
        }
        
        accumulator = (accumulator << 5) | DECODING[c];
        bit_count += 5;
        
        // Extract bytes when we have enough bits
        while (bit_count >= 8 && byte_index < 16) {
            bit_count -= 8;
            binary[byte_index++] = static_cast<uint8_t>((accumulator >> bit_count) & 0xFF);
        }
    }
    
    return binary;
}

// Convert 16-byte binary back to ULID string
std::string binaryToUlidString(const std::array<uint8_t, 16>& binary) {
    const char ENCODING[] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";
    std::string result;
    result.reserve(26);
    
    // Process in groups of 5 bytes (40 bits) to get 8 base32 characters
    for (size_t i = 0; i < 16; i += 5) {
        uint64_t chunk = 0;
        int bytes_in_chunk = std::min(5, static_cast<int>(16 - i));
        
        // Build 40-bit (or less) chunk
        for (int j = 0; j < bytes_in_chunk; ++j) {
            chunk = (chunk << 8) | binary[i + j];
        }
        
        // Extract base32 characters (8 for full chunk, fewer for partial)
        int chars_in_chunk = (bytes_in_chunk * 8 + 4) / 5; // Round up division
        for (int j = chars_in_chunk - 1; j >= 0; --j) {
            result.push_back(ENCODING[(chunk >> (j * 5)) & 0x1F]);
        }
    }
    
    // ULID should always be exactly 26 characters
    result.resize(26, '0');
    return result;
}

// Helper function to print binary data as hex
std::string binaryToHex(const std::array<uint8_t, 16>& binary) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (uint8_t byte : binary) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

void testUlidBinaryConversion() {
    std::cout << "=== ULID String ↔ Binary Conversion Test ===" << std::endl;
    
    // Generate some test ULIDs
    std::vector<std::string> test_ulids;
    for (int i = 0; i < 5; ++i) {
        test_ulids.push_back(generateULID());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    std::cout << "Original ULIDs → Binary → Back to String:" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    for (const auto& ulid : test_ulids) {
        // Convert to binary
        auto binary = ulidStringToBinary(ulid);
        
        // Convert back to string
        std::string restored = binaryToUlidString(binary);
        
        std::cout << "Original:  " << ulid << std::endl;
        std::cout << "Binary:    " << binaryToHex(binary) << std::endl;
        std::cout << "Restored:  " << restored << std::endl;
        std::cout << "Match:     " << (ulid == restored ? "✅ YES" : "❌ NO") << std::endl;
        std::cout << std::endl;
    }
}

void testBinarySorting() {
    std::cout << "=== Binary Sorting Test ===" << std::endl;
    
    // Generate ULIDs with time progression
    std::vector<std::string> ulid_strings;
    std::vector<std::array<uint8_t, 16>> ulid_binaries;
    
    for (int i = 0; i < 10; ++i) {
        std::string ulid = generateULID();
        ulid_strings.push_back(ulid);
        ulid_binaries.push_back(ulidStringToBinary(ulid));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    // Sort strings lexicographically
    std::vector<std::string> sorted_strings = ulid_strings;
    std::sort(sorted_strings.begin(), sorted_strings.end());
    
    // Sort binaries
    std::vector<std::array<uint8_t, 16>> sorted_binaries = ulid_binaries;
    std::sort(sorted_binaries.begin(), sorted_binaries.end());
    
    // Check if both sorts produce the same order
    bool order_matches = true;
    for (size_t i = 0; i < sorted_strings.size(); ++i) {
        auto expected_binary = ulidStringToBinary(sorted_strings[i]);
        if (expected_binary != sorted_binaries[i]) {
            order_matches = false;
            break;
        }
    }
    
    std::cout << "String vs Binary sorting comparison:" << std::endl;
    std::cout << "String sort order matches binary sort order: " 
              << (order_matches ? "✅ YES" : "❌ NO") << std::endl;
    
    std::cout << "\nSorted ULIDs (string format):" << std::endl;
    for (size_t i = 0; i < sorted_strings.size(); ++i) {
        std::cout << (i + 1) << ". " << sorted_strings[i] << std::endl;
    }
    
    std::cout << "\nSorted ULIDs (binary format as hex):" << std::endl;
    for (size_t i = 0; i < sorted_binaries.size(); ++i) {
        std::cout << (i + 1) << ". " << binaryToHex(sorted_binaries[i]) << std::endl;
    }
}

void demonstrateMySQLUsage() {
    std::cout << "\n=== MySQL BINARY(16) Usage Examples ===" << std::endl;
    
    // Generate some ULIDs
    std::vector<std::string> ulids;
    for (int i = 0; i < 5; ++i) {
        ulids.push_back(generateULID());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    std::cout << "SQL CREATE TABLE example:" << std::endl;
    std::cout << R"(
CREATE TABLE users (
    id BINARY(16) PRIMARY KEY,           -- ULID in binary format
    ulid_str CHAR(26) GENERATED ALWAYS AS 
        (UNHEX(HEX(id))) VIRTUAL,        -- String representation
    email VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    INDEX idx_id_binary (id),            -- Binary index (fast!)
    INDEX idx_ulid_str (ulid_str)        -- String index (if needed)
);
)" << std::endl;
    
    std::cout << "Sample INSERT statements:" << std::endl;
    for (size_t i = 0; i < ulids.size(); ++i) {
        auto binary = ulidStringToBinary(ulids[i]);
        std::cout << "INSERT INTO users (id, email) VALUES " 
                  << "(UNHEX('" << binaryToHex(binary) << "'), 'user" << (i+1) 
                  << "@example.com');" << std::endl;
    }
    
    std::cout << "\nSample SELECT with ordering:" << std::endl;
    std::cout << R"(
-- This will return results in chronological order due to ULID timestamp
SELECT HEX(id) as ulid_hex, email, created_at 
FROM users 
ORDER BY id;  -- Binary sorting = chronological order!

-- Convert binary back to ULID string format (if needed)
SELECT 
    id,
    -- Custom function to convert binary to ULID string would go here
    email
FROM users 
WHERE id > UNHEX('01234567890ABCDEF0123456789ABCDEF')
ORDER BY id;
)" << std::endl;
}

void performanceComparison() {
    std::cout << "\n=== Performance Comparison ===" << std::endl;
    
    const int TEST_COUNT = 100000;
    std::vector<std::string> ulid_strings;
    std::vector<std::array<uint8_t, 16>> ulid_binaries;
    
    // Generate test data
    std::cout << "Generating " << TEST_COUNT << " ULIDs..." << std::endl;
    for (int i = 0; i < TEST_COUNT; ++i) {
        std::string ulid = generateULID();
        ulid_strings.push_back(ulid);
        ulid_binaries.push_back(ulidStringToBinary(ulid));
    }
    
    // Test string sorting performance
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::string> strings_copy = ulid_strings;
    std::sort(strings_copy.begin(), strings_copy.end());
    auto end = std::chrono::high_resolution_clock::now();
    auto string_sort_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Test binary sorting performance
    start = std::chrono::high_resolution_clock::now();
    std::vector<std::array<uint8_t, 16>> binaries_copy = ulid_binaries;
    std::sort(binaries_copy.begin(), binaries_copy.end());
    end = std::chrono::high_resolution_clock::now();
    auto binary_sort_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Sorting " << TEST_COUNT << " ULIDs:" << std::endl;
    std::cout << "String sorting:  " << string_sort_time.count() << " μs" << std::endl;
    std::cout << "Binary sorting:  " << binary_sort_time.count() << " μs" << std::endl;
    std::cout << "Performance gain: " << std::fixed << std::setprecision(2) 
              << (static_cast<double>(string_sort_time.count()) / binary_sort_time.count()) 
              << "x faster with binary" << std::endl;
    
    // Memory usage comparison
    size_t string_memory = ulid_strings.size() * 26; // 26 bytes per string ULID
    size_t binary_memory = ulid_binaries.size() * 16; // 16 bytes per binary ULID
    
    std::cout << "\nMemory usage for " << TEST_COUNT << " ULIDs:" << std::endl;
    std::cout << "String format:   " << (string_memory / 1024) << " KB" << std::endl;
    std::cout << "Binary format:   " << (binary_memory / 1024) << " KB" << std::endl;
    std::cout << "Space savings:   " << std::fixed << std::setprecision(1) 
              << (100.0 * (string_memory - binary_memory) / string_memory) << "%" << std::endl;
}

int main() {
    std::cout << "ULID Binary Format Analysis" << std::endl;
    std::cout << "============================" << std::endl;
    
    testUlidBinaryConversion();
    testBinarySorting();
    demonstrateMySQLUsage();
    performanceComparison();
    
    std::cout << "\n=== Key Benefits of BINARY(16) Storage ===" << std::endl;
    std::cout << "✅ Maintains chronological sort order" << std::endl;
    std::cout << "✅ 38% less storage space (16 vs 26 bytes)" << std::endl;
    std::cout << "✅ Faster comparisons and sorting" << std::endl;
    std::cout << "✅ Efficient indexing in MySQL/databases" << std::endl;
    std::cout << "✅ Network transfer optimization" << std::endl;
    
    return 0;
}