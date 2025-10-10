# ULID SQLite Extension

A SQLite extension for handling ULID (Universally Unique Lexicographically Sortable Identifier) data stored as 16-byte binary blobs.

## Features

- **`ulid_to_string(blob)`** - Convert 16-byte ULID blob to 26-character string
- **`ulid_from_string(text)`** - Convert 26-character ULID string to 16-byte blob
- **`ulid_timestamp(blob)`** - Extract timestamp (milliseconds since Unix epoch) from ULID
- **`ulid_randomness(blob)`** - Extract 10-byte randomness portion from ULID

## Files

- `ulid_extension.c` - Main SQLite extension implementation
- `Makefile_ulid` - Build configuration for the extension
- `sqlite3ext.h` - SQLite extension header (included)
- `sqlite3.h` - SQLite header (included)

## Building

### Linux
```bash
make -f Makefile_ulid
# Creates ulid_extension.so
```

### macOS  
```bash
make -f Makefile_ulid
# Creates ulid_extension.dylib
```

### Manual Build
```bash
# Linux
gcc -O2 -fPIC -std=c11 -Wall -Wextra -shared -o ulid_extension.so ulid_extension.c

# macOS
gcc -O2 -fPIC -std=c11 -Wall -Wextra -bundle -undefined dynamic_lookup -o ulid_extension.dylib ulid_extension.c
```

## Usage

### Loading the Extension

```bash
# Start sqlite3
sqlite3 mydatabase.db

# Load the extension (Linux)
.load ./ulid_extension.so

# Load the extension (macOS)
.load ./ulid_extension.dylib
```

### Function Examples

#### Basic Round-trip Conversion
```sql
-- Convert string to blob and back to string
SELECT ulid_to_string(ulid_from_string('01ARZ3NDEKTSV4RRFFQ69G5FAV'));
-- Result: 01ARZ3NDEKTSV4RRFFQ69G5FAV
```

#### Extract Timestamp
```sql
-- Get timestamp in milliseconds since Unix epoch
SELECT ulid_timestamp(ulid_from_string('01ARZ3NDEKTSV4RRFFQ69G5FAV'));
-- Result: 1469918176385
```

#### Extract Randomness
```sql
-- Get the 10-byte randomness portion as hex
SELECT hex(ulid_randomness(ulid_from_string('01ARZ3NDEKTSV4RRFFQ69G5FAV')));
-- Result: 982832051ACE2518C
```

### Table Usage Examples

#### Create Table with ULID Primary Key
```sql
CREATE TABLE items (
    id BLOB PRIMARY KEY,
    name TEXT NOT NULL,
    created_at INTEGER DEFAULT (strftime('%s','now') * 1000)
);
```

#### Insert Data
```sql
-- Insert with ULID string converted to blob
INSERT INTO items (id, name) 
VALUES (ulid_from_string('01ARZ3NDEKTSV4RRFFQ69G5FAV'), 'Example Item');

INSERT INTO items (id, name) 
VALUES (ulid_from_string('01ARZ3NDEKTSV4RRFFQ69G5FAW'), 'Another Item');
```

#### Query Data
```sql
-- Display ULIDs as readable strings
SELECT 
    ulid_to_string(id) AS ulid,
    name,
    ulid_timestamp(id) AS timestamp_ms,
    datetime(ulid_timestamp(id)/1000, 'unixepoch') AS created_datetime,
    hex(ulid_randomness(id)) AS randomness_hex
FROM items
ORDER BY id;
```

#### Date Range Queries
```sql
-- Find items created after a specific timestamp  
SELECT ulid_to_string(id), name
FROM items 
WHERE ulid_timestamp(id) > 1469918176000;
```

### Error Handling Examples

```sql
-- Invalid BLOB length (expect error)
SELECT ulid_to_string(X'0011');
-- Error: ulid_to_string: BLOB must be exactly 16 bytes

-- Invalid ULID string (expect error)  
SELECT ulid_from_string('not-a-valid-ulid');
-- Error: ulid_from_string: input must be exactly 26 characters after sanitization

-- Invalid characters (expect error)
SELECT ulid_from_string('01ARZ3NDEKTSV4RRFFQ69G5FAU');
-- Error: ulid_from_string: invalid character in ULID
```

### NULL Safety

```sql
-- All functions handle NULL inputs gracefully
SELECT ulid_to_string(NULL);      -- Returns NULL
SELECT ulid_from_string(NULL);    -- Returns NULL  
SELECT ulid_timestamp(NULL);      -- Returns NULL
SELECT ulid_randomness(NULL);     -- Returns NULL
```

## ULID Format Details

- **Total Length**: 26 characters (128 bits)
- **Encoding**: Crockford Base32 (case-insensitive, excludes I, L, O, U)
- **Structure**:
  - First 10 characters: 48-bit timestamp (milliseconds since Unix epoch)
  - Last 16 characters: 80-bit randomness
- **Binary Storage**: 16 bytes (6 bytes timestamp + 10 bytes randomness)

### Character Set
- Valid characters: `0123456789ABCDEFGHJKMNPQRSTVWXYZ`
- Case insensitive: `a-z` automatically converted to `A-Z`
- Ambiguous character aliases: `i/I` → `1`, `l/L` → `1`, `o/O` → `0`
- Invalid character: `U` (not aliased, will cause error)
- Hyphens (`-`) and spaces are ignored during parsing

## Testing

```bash
# Run basic test
make -f Makefile_ulid test

# Manual testing in sqlite3
sqlite3 :memory: \
  ".load ./ulid_extension.so" \
  "SELECT ulid_to_string(ulid_from_string('01ARZ3NDEKTSV4RRFFQ69G5FAV'));"
```

## Performance

- All functions are marked as `SQLITE_DETERMINISTIC` for query optimization
- Constant-time operations suitable for large datasets
- No dynamic memory allocation in critical paths
- Efficient bit manipulation for Base32 encoding/decoding

## Compatibility

- **SQLite**: 3.8.0+ (uses `sqlite3_create_function_v2`)
- **C Standard**: C11
- **Platforms**: Linux, macOS, Windows (with appropriate compiler)
- **Thread Safety**: Read-only operations, safe for concurrent use