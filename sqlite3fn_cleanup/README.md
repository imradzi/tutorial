# SQLite3 clean_whitespace Extension

A SQLite3 loadable extension that provides a function to normalize whitespace in text strings.

## Description

The `clean_whitespace()` function takes a text string and:
- Replaces all whitespace characters (space, tab, carriage return, line feed, form feed, vertical tab) with single spaces
- Reduces multiple consecutive spaces to a single space
- Trims leading and trailing whitespace

## Prerequisites

- GCC 13.3.0 or compatible C compiler
- SQLite3 (command-line tool and headers)
- GNU Make

## Building the Extension

### Using Make (Recommended)

```bash
make
```

This will produce `clean_whitespace.so` (on Linux) or `clean_whitespace.dylib` (on macOS).

### Manual Build

```bash
cc -O2 -fPIC -std=c11 -Wall -Wextra -Werror -fvisibility=hidden -shared -o clean_whitespace.so clean_whitespace.c
```

## Usage

### Loading the Extension in SQLite3

#### Interactive Mode

```bash
sqlite3 your_database.db
```

Then in the SQLite prompt:
```sql
.load ./clean_whitespace
```

#### Command Line

```bash
sqlite3 your_database.db ".load ./clean_whitespace" "SELECT clean_whitespace('  hello    world  ');"
```

### Function Signature

```sql
clean_whitespace(text) -> text
```

### Examples

#### Basic Usage

```sql
-- Multiple spaces
SELECT clean_whitespace('hello    world');
-- Result: 'hello world'

-- Leading and trailing spaces
SELECT clean_whitespace('  trim me  ');
-- Result: 'trim me'

-- Mixed whitespace (tabs, newlines, spaces)
SELECT clean_whitespace('text	with
mixed   whitespace');
-- Result: 'text with mixed whitespace'
```

#### Using with Tables

```sql
-- Create a table with messy text
CREATE TABLE articles (id INTEGER, content TEXT);
INSERT INTO articles VALUES (1, '  This   has
  messy    formatting  ');

-- Clean the whitespace
SELECT id, clean_whitespace(content) AS cleaned_content 
FROM articles;
```

#### Updating Data

```sql
-- Clean whitespace in existing data
UPDATE articles 
SET content = clean_whitespace(content) 
WHERE content LIKE '%  %';
```

#### NULL Handling

```sql
SELECT clean_whitespace(NULL);
-- Result: NULL
```

## Testing

Run the comprehensive test suite:

```bash
make test
```

Or manually:

```bash
sqlite3 < test_clean_whitespace.sql
```

The test script covers:
- Multiple consecutive spaces
- Tabs and newlines
- Leading/trailing whitespace
- Mixed whitespace characters
- Empty strings
- NULL values
- Real-world multi-line text
- Table operations

## Implementation Details

- Written in C11
- Uses SQLite's memory management (`sqlite3_malloc`, `sqlite3_free`)
- Marked as `SQLITE_DETERMINISTIC` for query optimization
- Handles UTF-8 text
- Efficient single-pass algorithm
- Proper error handling for NULL and non-text inputs

## File Structure

```
sqlite3fn_cleanup/
├── clean_whitespace.c          # Extension source code
├── Makefile                    # Build configuration
├── test_clean_whitespace.sql   # Comprehensive test suite
└── README.md                   # This file
```

## Dependencies

The extension uses SQLite3 headers from the adjacent `sqlite3fn_ulid` directory:
- `../sqlite3fn_ulid/sqlite3ext.h`

## Cleaning Build Artifacts

```bash
make clean
```

This removes `.so`, `.dylib`, `.dll`, and `.o` files.

## Platform Support

- **Linux**: Builds as `.so` shared library
- **macOS**: Builds as `.dylib` dynamic library
- **Windows**: Should build as `.dll` (untested)

## License

This extension follows the same licensing as SQLite3.

## Author

Created for whitespace normalization in SQLite databases.
