#pragma once

#include <string>
#include <chrono>
#include <random>
#include <array>
#include <algorithm>
#include <stdexcept>
#include <cctype>

/**
 * ULID (Universally Unique Lexicographically Sortable Identifier) Generator
 * 
 * ULID Specification:
 * - 128-bit compatibility with UUID
 * - 1.21e+24 unique ULIDs per millisecond
 * - Lexicographically sortable!
 * - Canonically encoded as a 26 character string
 * - Uses Crockford's base32 for better efficiency and readability (5 bits per character)
 * - Case insensitive
 * - No special characters (URL safe)
 * - Monotonic sort order (correctly detects and handles the same millisecond)
 * 
 * Usage:
 *   #include "ulid.h"
 *   
 *   std::string id = generateULID();
 *   uint64_t timestamp = extractTimestampFromULID(id);
 */

class ULIDGenerator {
private:
    // Crockford's Base32 encoding characters (excludes I, L, O, U)
    static constexpr char ENCODING[] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";
    
    mutable std::random_device rd;
    mutable std::mt19937_64 gen;
    mutable std::uniform_int_distribution<uint64_t> dis;
    
    // For monotonic ULIDs within the same millisecond
    mutable uint64_t last_timestamp = 0;
    mutable std::array<uint8_t, 10> last_randomness = {};
    
public:
    ULIDGenerator() : gen(rd()), dis(0, UINT64_MAX) {}
    
    /**
     * Generate a ULID string
     * @return 26-character ULID string
     */
    std::string generate() const {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        
        std::array<uint8_t, 16> ulid_bytes = {};
        
        // Timestamp (48 bits = 6 bytes)
        for (int i = 5; i >= 0; --i) {
            ulid_bytes[i] = static_cast<uint8_t>(timestamp & 0xFF);
            timestamp >>= 8;
        }
        
        // Randomness (80 bits = 10 bytes)
        generateRandomness(ulid_bytes, now);
        
        // Encode to Base32
        return encode(ulid_bytes);
    }
    
    /**
     * Generate a ULID for a specific timestamp
     * @param timestamp_ms Timestamp in milliseconds since epoch
     * @return 26-character ULID string
     */
    std::string generate(uint64_t timestamp_ms) const {
        std::array<uint8_t, 16> ulid_bytes = {};
        
        // Timestamp (48 bits = 6 bytes)
        uint64_t ts = timestamp_ms;
        for (int i = 5; i >= 0; --i) {
            ulid_bytes[i] = static_cast<uint8_t>(ts & 0xFF);
            ts >>= 8;
        }
        
        // Generate random bytes for randomness part
        for (int i = 6; i < 16; ++i) {
            ulid_bytes[i] = static_cast<uint8_t>(dis(gen) & 0xFF);
        }
        
        return encode(ulid_bytes);
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
        // Increment randomness part (big-endian)
        for (int i = 9; i >= 0; --i) {
            if (++last_randomness[i] != 0) {
                break; // No carry needed
            }
            // Carry to next byte (overflow handled naturally)
        }
    }
    
    std::string encode(const std::array<uint8_t, 16>& data) const {
        std::string result;
        result.reserve(26);
        
        // Process in groups of 5 bytes (40 bits) to get 8 base32 characters
        for (size_t i = 0; i < 16; i += 5) {
            uint64_t chunk = 0;
            int bytes_in_chunk = std::min(5, static_cast<int>(16 - i));
            
            // Build 40-bit (or less) chunk
            for (int j = 0; j < bytes_in_chunk; ++j) {
                chunk = (chunk << 8) | data[i + j];
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
};

// Global ULID generator instance
static thread_local ULIDGenerator ulid_generator;

/**
 * Generate a new ULID
 * @return 26-character ULID string
 */
inline std::string generateULID() {
    return ulid_generator.generate();
}

/**
 * Generate a ULID for a specific timestamp
 * @param timestamp_ms Timestamp in milliseconds since epoch
 * @return 26-character ULID string
 */
inline std::string generateULID(uint64_t timestamp_ms) {
    return ulid_generator.generate(timestamp_ms);
}

/**
 * Extract timestamp from ULID string
 * @param ulid 26-character ULID string
 * @return timestamp in milliseconds since epoch
 */
inline uint64_t extractTimestampFromULID(const std::string& ulid) {
    if (ulid.length() != 26) {
        throw std::invalid_argument("Invalid ULID length");
    }
    
    // Decode first 10 characters (timestamp part)
    uint64_t timestamp = 0;
    const char DECODING[] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
        -1, 10, 11, 12, 13, 14, 15, 16, 17, -1, 18, 19, -1, 20, 21, -1,
        22, 23, 24, 25, 26, -1, 27, 28, 29, 30, 31, -1, -1, -1, -1, -1
    };
    
    for (int i = 0; i < 10; ++i) {
        char c = std::toupper(ulid[i]);
        if (c < 0 || c >= 96 || DECODING[c] == -1) {
            throw std::invalid_argument("Invalid character in ULID");
        }
        timestamp = (timestamp << 5) | DECODING[c];
    }
    
    return timestamp;
}

/**
 * Validate if a string is a valid ULID format
 * @param ulid String to validate
 * @return true if valid ULID format
 */
inline bool isValidULID(const std::string& ulid) {
    if (ulid.length() != 26) {
        return false;
    }
    
    const std::string valid_chars = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";
    for (char c : ulid) {
        if (valid_chars.find(std::toupper(c)) == std::string::npos) {
            return false;
        }
    }
    
    return true;
}

/**
 * Convert ULID string to 16-byte binary representation for BINARY(16) storage
 * @param ulid 26-character ULID string
 * @return 16-byte binary array suitable for database storage
 */
inline std::array<uint8_t, 16> ulidStringToBinary(const std::string& ulid) {
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
    
    // Decode 26 characters (130 bits) to 128 bits (16 bytes)
    uint64_t accumulator = 0;
    int bits_accumulated = 0;
    int byte_index = 0;
    
    for (char c : ulid) {
        if (c < 0 || c >= 128 || DECODING[c] == -1) {
            throw std::invalid_argument("Invalid character in ULID");
        }
        
        accumulator = (accumulator << 5) | DECODING[c];
        bits_accumulated += 5;
        
        // Extract complete bytes
        while (bits_accumulated >= 8 && byte_index < 16) {
            bits_accumulated -= 8;
            binary[byte_index++] = static_cast<uint8_t>((accumulator >> bits_accumulated) & 0xFF);
        }
    }
    
    return binary;
}

/**
 * Convert 16-byte binary representation back to ULID string
 * @param binary 16-byte binary array
 * @return 26-character ULID string
 */
inline std::string binaryToUlidString(const std::array<uint8_t, 16>& binary) {
    const char ENCODING[] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";
    std::string result;
    result.reserve(26);
    
    // Convert 128 bits (16 bytes) to 130 bits in base32 (26 characters)
    uint64_t accumulator = 0;
    int bits_accumulated = 0;
    
    for (uint8_t byte : binary) {
        accumulator = (accumulator << 8) | byte;
        bits_accumulated += 8;
        
        // Extract 5-bit groups
        while (bits_accumulated >= 5) {
            bits_accumulated -= 5;
            result.push_back(ENCODING[(accumulator >> bits_accumulated) & 0x1F]);
        }
    }
    
    // Handle remaining bits (should be exactly 2 bits left for 130 total)
    if (bits_accumulated > 0) {
        result.push_back(ENCODING[(accumulator << (5 - bits_accumulated)) & 0x1F]);
    }
    
    // Ensure exactly 26 characters
    result.resize(26, '0');
    return result;
}

/**
 * Convert binary ULID to hexadecimal string for debugging
 * @param binary 16-byte binary array
 * @return 32-character hex string
 */
inline std::string binaryToHex(const std::array<uint8_t, 16>& binary) {
    std::string result;
    result.reserve(32);
    const char hex_chars[] = "0123456789abcdef";
    
    for (uint8_t byte : binary) {
        result.push_back(hex_chars[byte >> 4]);
        result.push_back(hex_chars[byte & 0x0F]);
    }
    
    return result;
}
