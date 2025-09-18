#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <thread>
#include <chrono>
#include <mysqlx/xdevapi.h>
#include "ULID.h"

class UserRepository {
private:
    mysqlx::Session& session;
    
public:
    UserRepository(mysqlx::Session& sess) : session(sess) {}
    
    // Create user with ULID primary key
    ULID createUser(const std::string& email, const std::string& name) {
        ULID user_id;  // Generate new ULID
        
        try {
            // Use binary data directly - no string conversion needed!
            auto stmt = session.sql("INSERT INTO users (id, email, name) VALUES (?, ?, ?)")
                              .bind(mysqlx::Value(static_cast<const char*>(static_cast<const void*>(user_id.data())), user_id.size()))
                              .bind(email)
                              .bind(name);
            
            stmt.execute();
            
            std::cout << "✅ Created user: " << user_id << " (" << email << ")" << std::endl;
            return user_id;
            
        } catch (const mysqlx::Error& e) {
            std::cerr << "❌ MySQL Error in createUser: " << e.what() << std::endl;
            throw;
        }
    }
    
    // Find user by ULID
    bool findUser(const ULID& user_id, std::string& email, std::string& name) {
        try {
            auto stmt = session.sql("SELECT email, name FROM users WHERE id = ?")
                              .bind(mysqlx::Value(static_cast<const char*>(static_cast<const void*>(user_id.data())), user_id.size()));
            
            auto result = stmt.execute();
            
            if (auto row = result.fetchOne()) {
                email = row[0].get<std::string>();
                name = row[1].get<std::string>();
                return true;
            }
            
            return false;
            
        } catch (const mysqlx::Error& e) {
            std::cerr << "❌ MySQL Error in findUser: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Get users created after a specific timestamp
    std::vector<std::tuple<ULID, std::string, std::string>> getUsersAfter(uint64_t timestamp_ms) {
        std::vector<std::tuple<ULID, std::string, std::string>> users;
        
        try {
            // Create ULID with specific timestamp for range query
            ULID min_ulid = ULID::generate(timestamp_ms);
            
            auto stmt = session.sql("SELECT id, email, name FROM users WHERE id >= ? ORDER BY id")
                              .bind(mysqlx::Value(static_cast<const char*>(static_cast<const void*>(min_ulid.data())), min_ulid.size()));
            
            auto result = stmt.execute();
            
            for (auto row : result) {
                // Extract binary ULID from database
                auto id_bytes = row[0].get<std::string>();
                ULID user_id(reinterpret_cast<const uint8_t*>(id_bytes.data()));
                
                std::string email = row[1].get<std::string>();
                std::string name = row[2].get<std::string>();
                
                users.emplace_back(user_id, email, name);
            }
            
        } catch (const mysqlx::Error& e) {
            std::cerr << "❌ MySQL Error in getUsersAfter: " << e.what() << std::endl;
        }
        
        return users;
    }
    
    // Get all users ordered by creation time (ULID natural order)
    std::vector<std::tuple<ULID, std::string, std::string>> getAllUsersOrderedByTime() {
        std::vector<std::tuple<ULID, std::string, std::string>> users;
        
        try {
            // ORDER BY id gives us chronological order automatically!
            auto result = session.sql("SELECT id, email, name FROM users ORDER BY id").execute();
            
            for (auto row : result) {
                auto id_bytes = row[0].get<std::string>();
                ULID user_id(reinterpret_cast<const uint8_t*>(id_bytes.data()));
                
                std::string email = row[1].get<std::string>();
                std::string name = row[2].get<std::string>();
                
                users.emplace_back(user_id, email, name);
            }
            
        } catch (const mysqlx::Error& e) {
            std::cerr << "❌ MySQL Error in getAllUsersOrderedByTime: " << e.what() << std::endl;
        }
        
        return users;
    }
    
    // Create table with proper BINARY(16) column
    void createTable() {
        try {
            session.sql(R"(
                CREATE TABLE IF NOT EXISTS users (
                    id BINARY(16) PRIMARY KEY,
                    email VARCHAR(255) NOT NULL UNIQUE,
                    name VARCHAR(255) NOT NULL,
                    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                    
                    INDEX idx_email (email),
                    INDEX idx_created_at (created_at)
                )
            )").execute();
            
            std::cout << "✅ Table 'users' created/verified" << std::endl;
            
        } catch (const mysqlx::Error& e) {
            std::cerr << "❌ MySQL Error in createTable: " << e.what() << std::endl;
            throw;
        }
    }
};

void demonstrateULIDWithMySQL() {
    std::cout << "=== ULID + MySQL Integration Demo ===" << std::endl;
    
    try {
        // Connect to MySQL (adjust connection string as needed)
        const char* url = "mysqlx://root:ft1832h@127.0.0.1:33061/bkk_membership";
        std::cout << "Connecting to MySQL..." << std::endl;
        
        mysqlx::Client client(url);
        auto session = client.getSession();
        
        std::cout << "✅ Connected to MySQL!" << std::endl;
        
        UserRepository userRepo(session);
        
        // Create table
        userRepo.createTable();
        
        // Create some users with ULID primary keys
        std::cout << "\n--- Creating Users ---" << std::endl;
        std::vector<ULID> user_ids;
        
        user_ids.push_back(userRepo.createUser("john@example.com", "John Doe"));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        user_ids.push_back(userRepo.createUser("jane@example.com", "Jane Smith"));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        user_ids.push_back(userRepo.createUser("bob@example.com", "Bob Johnson"));
        
        // Find users by ULID
        std::cout << "\n--- Finding Users by ULID ---" << std::endl;
        for (const auto& user_id : user_ids) {
            std::string email, name;
            if (userRepo.findUser(user_id, email, name)) {
                std::cout << "Found: " << user_id << " -> " << name << " (" << email << ")" << std::endl;
            } else {
                std::cout << "❌ User not found: " << user_id << std::endl;
            }
        }
        
        // Get users ordered by creation time (ULID natural order)
        std::cout << "\n--- Users Ordered by Creation Time ---" << std::endl;
        auto ordered_users = userRepo.getAllUsersOrderedByTime();
        for (const auto& [id, email, name] : ordered_users) {
            std::cout << id << " | " << name << " | " << email 
                      << " | ts: " << id.timestamp() << std::endl;
        }
        
        // Range query by timestamp
        if (!user_ids.empty()) {
            std::cout << "\n--- Range Query by Timestamp ---" << std::endl;
            uint64_t middle_timestamp = user_ids[1].timestamp(); // Get timestamp from second user
            
            std::cout << "Searching for users created at or after timestamp: " << middle_timestamp << std::endl;
            auto recent_users = userRepo.getUsersAfter(middle_timestamp);
            
            std::cout << "Found " << recent_users.size() << " users:" << std::endl;
            for (const auto& [id, email, name] : recent_users) {
                std::cout << "  " << id << " | " << name << " | ts: " << id.timestamp() << std::endl;
            }
        }
        
        // Demonstrate efficient binary operations
        std::cout << "\n--- Binary Operations Performance ---" << std::endl;
        std::cout << "ULID storage: 16 bytes per ID" << std::endl;
        std::cout << "String equivalent: 26 bytes per ID" << std::endl;
        std::cout << "Space saving: 38.5% less storage" << std::endl;
        std::cout << "Comparison: Direct binary comparison (faster than string)" << std::endl;
        std::cout << "Sorting: Natural chronological order" << std::endl;
        
        session.close();
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
    }
}

void showSQLExamples() {
    std::cout << "\n=== SQL Examples with BINARY(16) ULIDs ===" << std::endl;
    
    std::cout << R"(
-- 1. CREATE TABLE with BINARY(16) for optimal performance
CREATE TABLE users (
    id BINARY(16) PRIMARY KEY,           -- ULID as primary key
    email VARCHAR(255) NOT NULL UNIQUE,
    name VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    INDEX idx_email (email),
    INDEX idx_created_at (created_at)
);

-- 2. INSERT with binary ULID (done via C++ code)
-- INSERT INTO users (id, email, name) VALUES (?, ?, ?)
-- where ? is bound to user_id.data() with size 16

-- 3. SELECT by ULID primary key (very fast)
-- SELECT email, name FROM users WHERE id = ?

-- 4. Range query by timestamp (using ULID's timestamp property)
-- SELECT id, email, name FROM users WHERE id >= ? ORDER BY id
-- where ? is ULID::generate(timestamp_ms)

-- 5. Get all users in chronological order (automatic!)
SELECT id, email, name FROM users ORDER BY id;  -- Natural chronological order

-- 6. Count users created in last hour
SELECT COUNT(*) FROM users 
WHERE id >= /* ULID with (now - 1 hour) timestamp */;

-- 7. Join tables using ULID foreign keys
CREATE TABLE orders (
    id BINARY(16) PRIMARY KEY,
    user_id BINARY(16) NOT NULL,
    amount DECIMAL(10,2),
    FOREIGN KEY (user_id) REFERENCES users(id),
    INDEX idx_user_id (user_id)
);

-- Fast join with binary comparison
SELECT u.name, u.email, o.amount 
FROM users u 
JOIN orders o ON u.id = o.user_id 
WHERE u.id = ?;

)" << std::endl;
}

int main() {
    std::cout << "MySQL + ULID Integration Example" << std::endl;
    std::cout << "================================" << std::endl;
    
    demonstrateULIDWithMySQL();
    showSQLExamples();
    
    std::cout << "\n=== Key Integration Benefits ===" << std::endl;
    std::cout << "✅ Direct binary storage (no string conversion)" << std::endl;
    std::cout << "✅ Zero-copy database operations" << std::endl;
    std::cout << "✅ Automatic chronological ordering" << std::endl;
    std::cout << "✅ Fast primary key operations" << std::endl;
    std::cout << "✅ Efficient range queries by timestamp" << std::endl;
    std::cout << "✅ 38% less storage vs string ULIDs" << std::endl;
    std::cout << "✅ Type-safe ULID operations in C++" << std::endl;
    
    return 0;
}