# ULID Class Usage Guide

This guide shows how to use the ULID class for optimal database performance with internal binary storage.

## Features

- **Internal binary storage**: 16 bytes instead of 26-character strings
- **String conversion only when needed**: For display, logging, debugging
- **Zero-copy database operations**: Direct binary data binding
- **Efficient comparisons**: Binary comparison is faster than string
- **Automatic chronological sorting**: ULIDs sort by creation time
- **Type-safe operations**: Compile-time safety
- **STL container compatible**: Can use in vectors, maps, sets, etc.

## Basic Usage

### Creating ULIDs

```cpp
#include "ULID.h"

// Generate new ULID
ULID id1;                                    // Default constructor
ULID id2 = ULID::generate();                 // Static method
ULID id3 = ULID::generate(1234567890123ULL); // With specific timestamp

// From string
ULID id4("01FJYWZ3RJM927XKDJGDR06REA");

// From binary data
std::array<uint8_t, 16> binary_data = {...};
ULID id5(binary_data);

// From hex string
ULID id6 = ULID::fromHex("01860F9551D1A5C3E03D6FCB5D6C5119");
```

### Accessing ULID Data

```cpp
ULID id;

// Get string representation (only when needed!)
std::string str = id.toString();
std::cout << id;  // Uses toString() automatically

// Get binary data for database operations
const uint8_t* binary = id.data();  // 16-byte pointer
size_t size = id.size();             // Always 16

// Get timestamp information
uint64_t timestamp_ms = id.timestamp();
auto time_point = id.timePoint();

// Convert to hex for debugging
std::string hex = id.toHex();

// Validate
bool valid = id.isValid();
```

## MySQL Integration

### Table Creation

```sql
CREATE TABLE users (
    id BINARY(16) PRIMARY KEY,           -- ULID stored as binary
    email VARCHAR(255) NOT NULL UNIQUE,
    name VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    INDEX idx_email (email)
);

CREATE TABLE orders (
    id BINARY(16) PRIMARY KEY,
    user_id BINARY(16) NOT NULL,
    amount DECIMAL(10,2),
    FOREIGN KEY (user_id) REFERENCES users(id),
    INDEX idx_user_id (user_id)
);
```

### C++ Database Operations

```cpp
class UserRepository {
private:
    mysqlx::Session& session;
    
public:
    // INSERT with binary ULID
    ULID createUser(const std::string& email, const std::string& name) {
        ULID user_id;  // Generate new ULID
        
        auto stmt = session.sql("INSERT INTO users (id, email, name) VALUES (?, ?, ?)")
                          .bind(mysqlx::Value(static_cast<const char*>(static_cast<const void*>(user_id.data())), user_id.size()))
                          .bind(email)
                          .bind(name);
        
        stmt.execute();
        return user_id;
    }
    
    // SELECT by ULID (fast primary key lookup)
    bool findUser(const ULID& user_id, std::string& email, std::string& name) {
        auto stmt = session.sql("SELECT email, name FROM users WHERE id = ?")
                          .bind(mysqlx::Value(static_cast<const char*>(static_cast<const void*>(user_id.data())), user_id.size()));
        
        auto result = stmt.execute();
        if (auto row = result.fetchOne()) {
            email = row[0].get<std::string>();
            name = row[1].get<std::string>();
            return true;
        }
        return false;
    }
    
    // Range query by timestamp
    std::vector<User> getUsersAfter(uint64_t timestamp_ms) {
        ULID min_ulid = ULID::generate(timestamp_ms);
        
        auto stmt = session.sql("SELECT id, email, name FROM users WHERE id >= ? ORDER BY id")
                          .bind(mysqlx::Value(static_cast<const char*>(static_cast<const void*>(min_ulid.data())), min_ulid.size()));
        
        auto result = stmt.execute();
        std::vector<User> users;
        
        for (auto row : result) {
            auto id_bytes = row[0].get<std::string>();
            ULID user_id(reinterpret_cast<const uint8_t*>(id_bytes.data()));
            users.emplace_back(user_id, row[1].get<std::string>(), row[2].get<std::string>());
        }
        
        return users;
    }
    
    // Get all users in chronological order (automatic sorting!)
    std::vector<User> getAllUsersOrdered() {
        auto result = session.sql("SELECT id, email, name FROM users ORDER BY id").execute();
        // Results are automatically in chronological order!
    }
};
```

## Performance Comparison

| Feature | String ULID | ULID Class |
|---------|-------------|------------|
| **Storage** | 26 bytes | 16 bytes (-38%) |
| **Memory** | Higher | Lower |
| **Comparison** | String compare | Binary compare (faster) |
| **Database binding** | String conversion | Direct binary (zero-copy) |
| **Sorting** | Character-by-char | Byte-by-byte (faster) |
| **Type safety** | String errors | Compile-time safe |

## Advanced Usage

### Using in STL Containers

```cpp
// Vector of ULIDs
std::vector<ULID> ids;
ids.emplace_back();  // Generate new ULID
std::sort(ids.begin(), ids.end());  // Efficient binary sorting

// Map with ULID keys
std::unordered_map<ULID, std::string> user_data;
user_data[ULID::generate()] = "John Doe";

// Set of ULIDs (automatically sorted)
std::set<ULID> unique_ids;
unique_ids.insert(ULID::generate());
```

### Comparison Operations

```cpp
ULID id1, id2;
std::this_thread::sleep_for(std::chrono::milliseconds(1));
ULID id3;

// All comparison operators work with binary comparison
bool equal = (id1 == id2);       // Fast binary comparison
bool older = (id1 < id3);        // Chronological comparison
bool newer = (id3 > id1);        // Chronological comparison

// Sort automatically gives chronological order
std::vector<ULID> ids = {id3, id1, id2};
std::sort(ids.begin(), ids.end());  // Now in chronological order
```

### Error Handling

```cpp
try {
    ULID id("INVALID_STRING");
} catch (const std::invalid_argument& e) {
    std::cout << "Invalid ULID string: " << e.what() << std::endl;
}

// Validate before construction
if (ULID::isValidString("01FJYWZ3RJM927XKDJGDR06REA")) {
    ULID id("01FJYWZ3RJM927XKDJGDR06REA");
}
```

## SQL Query Patterns

### Chronological Queries
```sql
-- Get all records in chronological order (no timestamp column needed!)
SELECT * FROM users ORDER BY id;

-- Get records from last hour
SELECT * FROM users WHERE id >= ? ORDER BY id;
-- where ? is ULID::generate(now - 1 hour)

-- Get records between two timestamps
SELECT * FROM users WHERE id BETWEEN ? AND ? ORDER BY id;
```

### Efficient Joins
```sql
-- Join with ULID foreign keys (very fast with binary comparison)
SELECT u.name, o.amount 
FROM users u 
JOIN orders o ON u.id = o.user_id 
WHERE u.id = ?;

-- Range join
SELECT u.name, COUNT(o.id) as order_count
FROM users u 
LEFT JOIN orders o ON u.id = o.user_id AND o.id >= ?
GROUP BY u.id, u.name;
```

### Pagination with ULIDs
```sql
-- Cursor-based pagination (better than OFFSET)
SELECT id, email, name FROM users 
WHERE id > ? 
ORDER BY id 
LIMIT 20;
-- where ? is the last ULID from previous page
```

## Best Practices

1. **Always use BINARY(16)** for ULID columns in database
2. **Convert to string only for display** - keep binary internally
3. **Use direct binary binding** for database operations
4. **Leverage chronological sorting** - `ORDER BY id` gives time order
5. **Use for pagination** - cursor-based with ULIDs is more efficient than OFFSET
6. **Primary keys and foreign keys** - ULIDs are excellent for both
7. **Avoid string operations** - work with ULID objects, convert to string only when needed

## Migration from String ULIDs

If you're currently using string ULIDs, here's how to migrate:

```cpp
// Before (string-based)
std::string ulid_str = generateULIDString();
stmt.bind(ulid_str);

// After (ULID class)
ULID ulid;  // Generate internally as binary
stmt.bind(mysqlx::Value(static_cast<const char*>(static_cast<const void*>(ulid.data())), ulid.size()));

// String conversion only when needed
std::cout << "Created user: " << ulid << std::endl;  // Converts to string for display
```

The ULID class provides all the benefits of ULIDs while optimizing for database performance and type safety.