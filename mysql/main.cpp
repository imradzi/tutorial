#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include <cstdlib>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <unistd.h>
#include <vector>
#include <mysqlx/xdevapi.h>
#include <iostream>
#include <chrono>
#include <string>
#include <random>
#include <iomanip>
#include <unordered_map>

typedef std::chrono::system_clock::time_point TimePoint;

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
std::string generateULID() {
    return ulid_generator.generate();
}

/**
 * Generate a ULID for a specific timestamp
 * @param timestamp_ms Timestamp in milliseconds since epoch
 * @return 26-character ULID string
 */
std::string generateULID(uint64_t timestamp_ms) {
    return ulid_generator.generate(timestamp_ms);
}

/**
 * Extract timestamp from ULID string
 * @param ulid 26-character ULID string
 * @return timestamp in milliseconds since epoch
 */
uint64_t extractTimestampFromULID(const std::string& ulid) {
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

std::chrono::system_clock::time_point toTimePoint(const std::string& mysql_time) {
    if (mysql_time.empty()) {
        throw std::runtime_error("Cannot parse empty MySQL timestamp string");
    }

    // Check if the timestamp includes microseconds
    size_t microsecond_pos = mysql_time.find('.');
    std::string base_time = mysql_time;
    int microseconds = 0;

    if (microsecond_pos != std::string::npos) {
        base_time = mysql_time.substr(0, microsecond_pos);
        std::string microsecond_str = mysql_time.substr(microsecond_pos + 1);

        // Pad or truncate to 6 digits
        if (microsecond_str.length() < 6) {
            microsecond_str.append(6 - microsecond_str.length(), '0');
        } else if (microsecond_str.length() > 6) {
            microsecond_str = microsecond_str.substr(0, 6);
        }

        try {
            microseconds = std::stoi(microsecond_str);
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid microseconds in MySQL timestamp: " + mysql_time);
        }
    }

    std::tm tm = {};
    std::istringstream ss(base_time);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    if (ss.fail()) {
        throw std::runtime_error("Failed to parse MySQL timestamp: " + mysql_time);
    }

    // Use timegm for UTC interpretation to be consistent with mysqlValueToTimePoint
    time_t time_t_val = timegm(&tm);
    if (time_t_val == static_cast<time_t>(-1)) {
        throw std::runtime_error("Failed to convert parsed time to time_t: " + mysql_time);
    }

    auto time_point = std::chrono::system_clock::from_time_t(time_t_val);

    // Add microseconds if present
    if (microseconds > 0) {
        time_point += std::chrono::microseconds(microseconds);
    }

    return time_point;
}

template<typename T>
T get(const mysqlx::Value& value, T default_value) {
    if (value.isNull()) {
        return default_value;
    }
    if constexpr (std::is_same_v<T, TimePoint>) {
        return toTimePoint(value.get<std::string>());
    }
    return value.get<T>();
}

using namespace std::string_literals;


auto getColumnMap(const mysqlx::SqlResult& rs) {
    std::unordered_map<std::string, size_t> map;
    for (int i = 0; i < rs.getColumnCount(); i++) {
        map[rs.getColumn(i).getColumnName()] = i;
    }
    return map;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            auto e = std::getenv(argv[i]);
            if (e) std::cout << argv[i] << ": " << e << std::endl;
        }
    }
    
    // Demonstrate ULID generation
    std::cout << "=== ULID Generation Demo ===" << std::endl;
    
    // Generate some ULIDs
    for (int i = 0; i < 5; ++i) {
        std::string ulid = generateULID();
        uint64_t timestamp = extractTimestampFromULID(ulid);
        
        // Convert timestamp back to human-readable format
        auto time_point = std::chrono::system_clock::time_point(
            std::chrono::milliseconds(timestamp)
        );
        auto time_t_val = std::chrono::system_clock::to_time_t(time_point);
        
        std::cout << "ULID " << (i+1) << ": " << ulid 
                  << " | Timestamp: " << timestamp
                  << " | Time: " << std::put_time(std::gmtime(&time_t_val), "%Y-%m-%d %H:%M:%S UTC")
                  << std::endl;
        
        // Small delay to show timestamp progression
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    // Generate multiple ULIDs in the same millisecond to show monotonic behavior
    std::cout << "\n=== Monotonic ULIDs (same millisecond) ===" << std::endl;
    auto start_time = std::chrono::steady_clock::now();
    std::vector<std::string> rapid_ulids;
    
    for (int i = 0; i < 10; ++i) {
        rapid_ulids.push_back(generateULID());
    }
    
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    
    for (size_t i = 0; i < rapid_ulids.size(); ++i) {
        std::cout << "Rapid ULID " << (i+1) << ": " << rapid_ulids[i] << std::endl;
    }
    
    std::cout << "Generated 10 ULIDs in " << duration << " microseconds" << std::endl;
    
    // Verify lexicographic ordering
    std::vector<std::string> sorted_ulids = rapid_ulids;
    std::sort(sorted_ulids.begin(), sorted_ulids.end());
    
    bool is_sorted = (rapid_ulids == sorted_ulids);
    std::cout << "ULIDs are lexicographically sorted: " << (is_sorted ? "YES" : "NO") << std::endl;
    
    std::cout << std::endl;
    try {
        const char* url = "mysqlx://root:ft1832h@127.0.0.1:33061/bkk_membership";
        // const char* url = "mysqlx://root:newpassword123@localhost"; --- this can connect

        std::cout << "Creating session on " << url << " ..." << std::endl;
        mysqlx::Client client(url);
        auto sess = client.getSession();

        std::cout << "Successfully connected to MySQL Server!" << std::endl;

        // Example: Generate ULIDs for potential database operations
        std::cout << "\n=== ULID for Database Operations ===" << std::endl;
        
        // Generate ULIDs that could be used as primary keys
        std::vector<std::string> record_ulids;
        for (int i = 0; i < 3; ++i) {
            std::string ulid = generateULID();
            record_ulids.push_back(ulid);
            std::cout << "Generated ULID for record " << (i+1) << ": " << ulid << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        // You could use these ULIDs in INSERT statements like:
        // INSERT INTO some_table (id, data) VALUES (?, ?)
        // where the first parameter would be one of these ULIDs
        
        std::cout << "\n=== Database Query Results ===" << std::endl;
        auto rs = sess.sql(
                          "SET FOREIGN_KEY_CHECKS = 0; \n"
                          "-- comment --\n"
                          "select id, state_code, name "
                          " -- another comment\n"
                          "from address_state;")
                      .execute();
        auto colMap = getColumnMap(rs);

        for (auto x : rs) {
            // Generate a ULID for each row as an example of how you might add ULIDs to existing data
            std::string row_ulid = generateULID();
            std::cout << "Row ULID: " << row_ulid << " | "
                      << get(x[colMap["id"]], 0) << "> " 
                      << get(x[colMap["state_code"]], ""s) << "> " 
                      << get(x[colMap["name"]], ""s) << std::endl;
        }
        sess.close();

    } catch (const mysqlx::Error& err) {
        std::cerr << "MySQLX Error: " << err.what() << std::endl;
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "Standard Exception: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
