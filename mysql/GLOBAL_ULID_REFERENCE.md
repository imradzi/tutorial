# Global ULID Functions Reference

This guide shows how to create and use global ULID functions accessible from all MySQL schemas.

## 🚀 Quick Setup

### Method 1: System Database (Recommended)

```bash
# Run the setup script
./setup_global_ulid_functions.sh

# Or manually:
mysql -u root -p < mysql_global_ulid_functions.sql
```

**Advantages:**
- ✅ Available globally without schema prefix
- ✅ System-level integration
- ✅ Survives schema changes

**Usage:**
```sql
SELECT mysql.ULID_TO_BIN('01FJYWZ3RJM927XKDJGDR06REA');
SELECT mysql.BIN_TO_ULID(binary_data);
SELECT mysql.ULID_TIMESTAMP('01FJYWZ3RJM927XKDJGDR06REA');
```

### Method 2: Dedicated Schema

```bash
mysql -u root -p < mysql_shared_ulid_schema.sql
```

**Usage:**
```sql
SELECT ulid_functions.ULID_TO_BIN('01FJYWZ3RJM927XKDJGDR06REA');
SELECT ulid_functions.BIN_TO_ULID(binary_data);
```

## 📋 Available Functions

### `mysql.ULID_TO_BIN(ulid_string)`
Convert 26-character ULID string to 16-byte binary format.

```sql
-- Convert ULID string to binary for storage
INSERT INTO users (id, name) VALUES 
    (mysql.ULID_TO_BIN('01FJYWZ3RJM927XKDJGDR06REA'), 'John Doe');
```

### `mysql.BIN_TO_ULID(binary_data)`
Convert 16-byte binary data back to 26-character ULID string.

```sql
-- Display binary ULIDs as readable strings
SELECT mysql.BIN_TO_ULID(id) as ulid_string, name 
FROM users ORDER BY id;
```

### `mysql.ULID_TIMESTAMP(ulid_string)`
Extract timestamp (milliseconds since epoch) from ULID string.

```sql
-- Get creation timestamp from ULID
SELECT mysql.ULID_TIMESTAMP('01FJYWZ3RJM927XKDJGDR06REA') as timestamp_ms;
-- Returns: 408818017220
```

### `mysql.ULID_TO_DATETIME(ulid_string)`
Convert ULID to MySQL DATETIME with millisecond precision.

```sql
-- Convert ULID to human-readable datetime
SELECT mysql.ULID_TO_DATETIME('01FJYWZ3RJM927XKDJGDR06REA') as created_at;
-- Returns: 1971-12-30 15:26:58.000
```

### `mysql.ULID_GENERATE(timestamp_ms)` ⚠️
Generate ULID for specific timestamp (simplified version).

```sql
-- Generate ULID for specific timestamp (limited randomness)
SELECT mysql.ULID_GENERATE(UNIX_TIMESTAMP(NOW(3)) * 1000);
```

**⚠️ Note:** Use C++ ULID class for production-grade random generation.

## 🎯 Practical Usage Patterns

### 1. Table Creation with Binary ULIDs

```sql
-- Any schema can use global functions
USE your_app_schema;

CREATE TABLE users (
    id BINARY(16) PRIMARY KEY,
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

### 2. Insert with C++ ULID Class

```cpp
// C++ code - insert binary ULID directly
ULID user_id;  // Generated as binary internally

auto stmt = session.sql("INSERT INTO users (id, email, name) VALUES (?, ?, ?)")
                  .bind(mysqlx::Value(static_cast<const char*>(static_cast<const void*>(user_id.data())), user_id.size()))
                  .bind("john@example.com")
                  .bind("John Doe");
stmt.execute();
```

### 3. Queries with String Conversion

```sql
-- Convert binary ULIDs to strings for display
SELECT 
    mysql.BIN_TO_ULID(id) as user_id,
    email,
    name,
    created_at
FROM users 
ORDER BY id;  -- Natural chronological order

-- Join tables with ULID foreign keys
SELECT 
    mysql.BIN_TO_ULID(u.id) as user_id,
    u.name,
    mysql.BIN_TO_ULID(o.id) as order_id,
    o.amount
FROM users u
JOIN orders o ON u.id = o.user_id
ORDER BY o.id;  -- Chronological order by order creation
```

### 4. Range Queries by Timestamp

```sql
-- Get users created in the last hour
SELECT mysql.BIN_TO_ULID(id) as user_id, name, email
FROM users 
WHERE id >= mysql.ULID_TO_BIN(
    mysql.ULID_GENERATE(
        (UNIX_TIMESTAMP(NOW(3)) - 3600) * 1000
    )
)
ORDER BY id;

-- Get users created between specific dates
SELECT mysql.BIN_TO_ULID(id) as user_id, name
FROM users 
WHERE id BETWEEN 
    mysql.ULID_TO_BIN('01FJYWZ0000000000000000000')  -- Start of range
    AND 
    mysql.ULID_TO_BIN('01FJYWZZZZZZZZZZZZZZZZZZZZ')   -- End of range
ORDER BY id;
```

### 5. Efficient Pagination

```sql
-- Cursor-based pagination (better than OFFSET)
SELECT mysql.BIN_TO_ULID(id) as user_id, name, email
FROM users 
WHERE id > mysql.ULID_TO_BIN(:last_seen_ulid)
ORDER BY id 
LIMIT 20;
```

### 6. Reporting Queries

```sql
-- Group by time periods using ULID timestamps
SELECT 
    DATE(mysql.ULID_TO_DATETIME(mysql.BIN_TO_ULID(id))) as creation_date,
    COUNT(*) as user_count
FROM users
GROUP BY DATE(mysql.ULID_TO_DATETIME(mysql.BIN_TO_ULID(id)))
ORDER BY creation_date;

-- Find users created within specific time windows
SELECT mysql.BIN_TO_ULID(id) as user_id, name
FROM users
WHERE mysql.ULID_TIMESTAMP(mysql.BIN_TO_ULID(id)) 
    BETWEEN 1640995200000  -- 2022-01-01 00:00:00 UTC in ms
    AND     1672531199000; -- 2022-12-31 23:59:59 UTC in ms
```

## 🔧 Administration

### Check Installed Functions

```sql
-- List all ULID functions
SELECT ROUTINE_NAME, ROUTINE_TYPE, ROUTINE_COMMENT
FROM INFORMATION_SCHEMA.ROUTINES
WHERE ROUTINE_SCHEMA = 'mysql'
  AND (ROUTINE_NAME LIKE 'ULID%' OR ROUTINE_NAME LIKE '%ULID');
```

### Grant Access to Users

```sql
-- Grant EXECUTE permission to specific users
GRANT EXECUTE ON FUNCTION mysql.ULID_TO_BIN TO 'app_user'@'%';
GRANT EXECUTE ON FUNCTION mysql.BIN_TO_ULID TO 'app_user'@'%';
GRANT EXECUTE ON FUNCTION mysql.ULID_TIMESTAMP TO 'app_user'@'%';
GRANT EXECUTE ON FUNCTION mysql.ULID_TO_DATETIME TO 'app_user'@'%';

-- Or grant to all users (already done in setup)
GRANT EXECUTE ON FUNCTION mysql.ULID_TO_BIN TO '%'@'%';
```

### Remove Functions

```sql
-- Remove global functions if needed
USE mysql;
DROP FUNCTION IF EXISTS ULID_TO_BIN;
DROP FUNCTION IF EXISTS BIN_TO_ULID;
DROP FUNCTION IF EXISTS ULID_TIMESTAMP;
DROP FUNCTION IF EXISTS ULID_TO_DATETIME;
DROP FUNCTION IF EXISTS ULID_GENERATE;
```

## ⚡ Performance Benefits

| Operation | Without Global Functions | With Global Functions |
|-----------|-------------------------|----------------------|
| **ULID Storage** | VARCHAR(26) - 26 bytes | BINARY(16) - 16 bytes |
| **String Conversion** | Always stored as string | Only when displaying |
| **Database Size** | Larger indexes | Smaller, faster indexes |
| **Sorting** | String comparison | Binary comparison (faster) |
| **Joins** | String matching | Binary matching (much faster) |
| **Range Queries** | String lexicographic | Binary + timestamp logic |

## 🎯 Integration with C++ ULID Class

Perfect combination: **C++ generates binary, MySQL functions handle string conversion**

```cpp
// C++ - Generate and use binary ULIDs
ULID user_id;                    // Binary generation
auto binary = user_id.data();    // Direct binary access
auto str = user_id.toString();   // String only when needed

// MySQL - Convert binary to string only for display
SELECT mysql.BIN_TO_ULID(id), name FROM users;
```

## 🔍 Troubleshooting

### Common Issues

1. **Functions not found**: Ensure you're prefixing with `mysql.`
2. **Permission denied**: Check EXECUTE grants
3. **Invalid ULID**: Validate 26-character length and valid Base32 characters
4. **NULL results**: Check for proper ULID format and non-empty inputs

### Test Installation

```sql
-- Quick test
SELECT mysql.ULID_TO_BIN('01FJYWZ3RJM927XKDJGDR06REA') IS NOT NULL as test1;
SELECT mysql.BIN_TO_ULID(mysql.ULID_TO_BIN('01FJYWZ3RJM927XKDJGDR06REA')) = '01FJYWZ3RJM927XKDJGDR06REA' as test2;
```

Both should return `1` (true) if functions work correctly.

---

**🎉 Result**: Global ULID functions available across all schemas for optimal binary storage with string conversion only when needed!