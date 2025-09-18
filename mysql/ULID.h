#pragma once

#include <string>
#include <chrono>
#include <random>
#include <array>
#include <algorithm>
#include <stdexcept>
#include <cctype>
#include <iostream>
#include <iomanip>
#include <cstring>

/**
 * ULID class with internal binary storage for optimal database performance
 * 
 * Features:
 * - Stores ULID as 16-byte binary internally
 * - Converts to string only when needed (display, logging)
 * - Direct binary data access for database operations
 * - Efficient comparisons and sorting
 * - Type-safe operations
 * 
 * Usage:
 *   ULID id;                           // Generate new ULID
 *   ULID id("01FJYWZ3RJM927XKDJGDR.."); // From string
 *   std::string str = id.toString();   // Convert to string
 *   auto binary = id.data();           // Get binary data for database
 */
class ULID {
private:
    std::array<uint8_t, 16> binary_data;
    
    // Static encoding/decoding tables for Base32
    static constexpr char ENCODING[] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";
    static const int8_t DECODING[128];
    
    // Static generator for new ULIDs
    static thread_local class ULIDGenerator {
    private:
        mutable std::random_device rd;
        mutable std::mt19937_64 gen;
        mutable std::uniform_int_distribution<uint64_t> dis;
        mutable uint64_t last_timestamp = 0;
        mutable std::array<uint8_t, 10> last_randomness = {};
        
    public:
        ULIDGenerator() : gen(rd()), dis(0, UINT64_MAX) {}
        
        std::array<uint8_t, 16> generate() const {
            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()).count();
            
            std::array<uint8_t, 16> ulid_bytes = {};
            
            // Timestamp (48 bits = 6 bytes) - big endian
            for (int i = 5; i >= 0; --i) {
                ulid_bytes[i] = static_cast<uint8_t>(timestamp & 0xFF);
                timestamp >>= 8;
            }
            
            // Randomness with monotonic guarantee
            generateRandomness(ulid_bytes, now);
            return ulid_bytes;
        }
        
    private:
        void generateRandomness(std::array<uint8_t, 16>& ulid_bytes, 
                               const std::chrono::system_clock::time_point& now) const {
            uint64_t current_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()).count();
            
            if (current_timestamp == last_timestamp) {
                // Same millisecond: increment last randomness for monotonic ordering
                incrementRandomness();
                std::copy(last_randomness.begin(), last_randomness.end(), 
                         ulid_bytes.begin() + 6);
            } else {
                // New millisecond: generate fresh randomness
                for (int i = 0; i < 10; ++i) {
                    last_randomness[i] = static_cast<uint8_t>(dis(gen) & 0xFF);
                    ulid_bytes[6 + i] = last_randomness[i];
                }
                last_timestamp = current_timestamp;
            }
        }
        
        void incrementRandomness() const {
            for (int i = 9; i >= 0; --i) {
                if (++last_randomness[i] != 0) {
                    break; // No carry needed
                }
            }
        }
    } generator;
    
public:
    /**
     * Default constructor - generates new ULID
     */
    ULID() : binary_data(generator.generate()) {}
    
    /**
     * Constructor from string representation
     * @param ulid_string 26-character ULID string
     */
    explicit ULID(const std::string& ulid_string) {
        if (ulid_string.length() != 26) {
            throw std::invalid_argument("Invalid ULID string length");
        }
        
        binary_data = {};
        uint64_t accumulator = 0;
        int bits_accumulated = 0;
        int byte_index = 0;
        
        for (char c : ulid_string) {
            if (c < 0 || c >= 128 || DECODING[c] == -1) {
                throw std::invalid_argument("Invalid character in ULID string");
            }
            
            accumulator = (accumulator << 5) | DECODING[c];
            bits_accumulated += 5;
            
            while (bits_accumulated >= 8 && byte_index < 16) {
                bits_accumulated -= 8;
                binary_data[byte_index++] = static_cast<uint8_t>((accumulator >> bits_accumulated) & 0xFF);
            }
        }
    }
    
    /**
     * Constructor from binary data
     * @param binary_ulid 16-byte binary ULID
     */
    explicit ULID(const std::array<uint8_t, 16>& binary_ulid) : binary_data(binary_ulid) {}
    
    /**
     * Constructor from raw binary data
     * @param data Pointer to 16 bytes of ULID data
     */
    explicit ULID(const uint8_t* data) {
        std::memcpy(binary_data.data(), data, 16);
    }
    
    /**
     * Copy constructor
     */
    ULID(const ULID& other) = default;
    
    /**
     * Assignment operator
     */
    ULID& operator=(const ULID& other) = default;
    
    /**
     * Convert to string representation (for display/logging)
     * @return 26-character ULID string
     */
    std::string toString() const {
        std::string result;
        result.reserve(26);
        
        uint64_t accumulator = 0;
        int bits_accumulated = 0;
        
        for (uint8_t byte : binary_data) {
            accumulator = (accumulator << 8) | byte;
            bits_accumulated += 8;
            
            while (bits_accumulated >= 5) {
                bits_accumulated -= 5;
                result.push_back(ENCODING[(accumulator >> bits_accumulated) & 0x1F]);
            }
        }
        
        if (bits_accumulated > 0) {
            result.push_back(ENCODING[(accumulator << (5 - bits_accumulated)) & 0x1F]);
        }
        
        result.resize(26, '0');
        return result;
    }
    
    /**
     * Get binary data for database operations
     * @return Pointer to 16-byte binary data
     */
    const uint8_t* data() const {
        return binary_data.data();
    }
    
    /**
     * Get binary data as array
     * @return 16-byte binary array
     */
    const std::array<uint8_t, 16>& binary() const {
        return binary_data;
    }
    
    /**
     * Get size in bytes (always 16)
     * @return 16
     */
    constexpr size_t size() const {
        return 16;
    }
    
    /**
     * Extract timestamp from ULID
     * @return Timestamp in milliseconds since epoch
     */
    uint64_t timestamp() const {
        uint64_t ts = 0;
        for (int i = 0; i < 6; ++i) {
            ts = (ts << 8) | binary_data[i];
        }
        return ts;
    }
    
    /**
     * Get timestamp as time_point
     * @return std::chrono::system_clock::time_point
     */
    std::chrono::system_clock::time_point timePoint() const {
        return std::chrono::system_clock::time_point(
            std::chrono::milliseconds(timestamp())
        );
    }
    
    /**
     * Convert to hexadecimal string for debugging
     * @return 32-character hex string
     */
    std::string toHex() const {
        std::string result;
        result.reserve(32);
        const char hex_chars[] = "0123456789abcdef";
        
        for (uint8_t byte : binary_data) {
            result.push_back(hex_chars[byte >> 4]);
            result.push_back(hex_chars[byte & 0x0F]);
        }
        return result;
    }
    
    /**
     * Check if ULID is valid (non-zero)
     * @return true if valid
     */
    bool isValid() const {
        return std::any_of(binary_data.begin(), binary_data.end(), 
                          [](uint8_t b) { return b != 0; });
    }
    
    // Comparison operators (efficient binary comparison)
    bool operator==(const ULID& other) const {
        return binary_data == other.binary_data;
    }
    
    bool operator!=(const ULID& other) const {
        return binary_data != other.binary_data;
    }
    
    bool operator<(const ULID& other) const {
        return binary_data < other.binary_data;
    }
    
    bool operator<=(const ULID& other) const {
        return binary_data <= other.binary_data;
    }
    
    bool operator>(const ULID& other) const {
        return binary_data > other.binary_data;
    }
    
    bool operator>=(const ULID& other) const {
        return binary_data >= other.binary_data;
    }
    
    // Stream operators
    friend std::ostream& operator<<(std::ostream& os, const ULID& ulid) {
        return os << ulid.toString();
    }
    
    friend std::istream& operator>>(std::istream& is, ULID& ulid) {
        std::string str;
        is >> str;
        ulid = ULID(str);
        return is;
    }
    
    // Static utility functions
    
    /**
     * Generate new ULID
     * @return New ULID instance
     */
    static ULID generate() {
        return ULID();
    }
    
    /**
     * Generate ULID with specific timestamp
     * @param timestamp_ms Timestamp in milliseconds since epoch
     * @return ULID with specified timestamp
     */
    static ULID generate(uint64_t timestamp_ms) {
        std::array<uint8_t, 16> ulid_bytes = {};
        
        // Set timestamp (48 bits = 6 bytes)
        uint64_t ts = timestamp_ms;
        for (int i = 5; i >= 0; --i) {
            ulid_bytes[i] = static_cast<uint8_t>(ts & 0xFF);
            ts >>= 8;
        }
        
        // Generate random bytes
        thread_local std::random_device rd;
        thread_local std::mt19937_64 gen(rd());
        thread_local std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);
        
        for (int i = 6; i < 16; ++i) {
            ulid_bytes[i] = static_cast<uint8_t>(dis(gen) & 0xFF);
        }
        
        return ULID(ulid_bytes);
    }
    
    /**
     * Create ULID from hex string
     * @param hex_string 32-character hex string
     * @return ULID instance
     */
    static ULID fromHex(const std::string& hex_string) {
        if (hex_string.length() != 32) {
            throw std::invalid_argument("Invalid hex string length");
        }
        
        std::array<uint8_t, 16> binary_data;
        for (size_t i = 0; i < 16; ++i) {
            std::string byte_str = hex_string.substr(i * 2, 2);
            binary_data[i] = static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16));
        }
        
        return ULID(binary_data);
    }
    
    /**
     * Validate ULID string format
     * @param ulid_string String to validate
     * @return true if valid format
     */
    static bool isValidString(const std::string& ulid_string) {
        if (ulid_string.length() != 26) {
            return false;
        }
        
        for (char c : ulid_string) {
            if (c < 0 || c >= 128 || DECODING[c] == -1) {
                return false;
            }
        }
        
        return true;
    }
};

// Static member definitions
constexpr char ULID::ENCODING[];

const int8_t ULID::DECODING[128] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, 16, 17, -1, 18, 19, -1, 20, 21, -1,
    22, 23, 24, 25, 26, -1, 27, 28, 29, 30, 31, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, 16, 17, -1, 18, 19, -1, 20, 21, -1,
    22, 23, 24, 25, 26, -1, 27, 28, 29, 30, 31, -1, -1, -1, -1, -1
};

thread_local ULID::ULIDGenerator ULID::generator;

// Hash support for std::unordered_map, std::unordered_set
namespace std {
    template<>
    struct hash<ULID> {
        size_t operator()(const ULID& ulid) const {
            // Use first 8 bytes as hash (sufficient for most use cases)
            const uint8_t* data = ulid.data();
            size_t result = 0;
            for (int i = 0; i < 8; ++i) {
                result = (result << 8) | data[i];
            }
            return result;
        }
    };
}