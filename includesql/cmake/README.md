# SQL Generation Module

This module provides CMake functions to automatically generate C++ source files from SQL files, enabling you to embed SQL queries directly into your application with proper escaping and efficient incremental builds.

## Features

- ✅ **Automatic C++/Header Generation**: Converts `.sql` files to `.cpp` and `.h` files
- ✅ **Incremental Builds**: Only regenerates files when SQL content changes
- ✅ **Proper Escaping**: Handles quotes, newlines, and special characters correctly
- ✅ **Custom Variable Names**: Uses `bkk_` prefix (configurable per file)
- ✅ **Error Validation**: Checks file existence and parameter consistency
- ✅ **Clean API**: Simple functions for easy integration

## Quick Start

### 1. Basic Setup

```cmake
# Include the module
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(SqlGeneration)

# Generate SQL sources (one-liner)
setup_sql_generation(
    TARGET_NAME generate_sql_sources
    OUTPUT_SOURCES GENERATED_SOURCES
    SQL_FILES "user_queries.sql" "product_queries.sql"
    CPP_FILES "bkk_users.cpp" "bkk_products.cpp"
    VARIABLE_NAMES "bkk_USER_QUERIES" "bkk_PRODUCT_QUERIES"
)

# Use in your executable
add_executable(my_app main.cpp ${GENERATED_SOURCES})
add_dependencies(my_app generate_sql_sources)
```

### 2. Generated Files Structure

**Input:** `user_queries.sql`
```sql
SELECT * FROM users WHERE active = TRUE;
```

**Generated:** `bkk_users.h`
```cpp
#pragma once
extern const char* bkk_USER_QUERIES;
```

**Generated:** `bkk_users.cpp`  
```cpp
#include "bkk_users.h"
const char* bkk_USER_QUERIES = "SELECT * FROM users WHERE active = TRUE;";
```

**Usage in your code:**
```cpp
#include "bkk_users.h"
// Use bkk_USER_QUERIES with your database library
```

## API Reference

### `setup_sql_generation()`

Complete setup for SQL generation (recommended for most cases).

```cmake
setup_sql_generation(
    TARGET_NAME <target_name>
    OUTPUT_SOURCES <variable_name>
    SQL_FILES <file1.sql> <file2.sql> ...
    CPP_FILES <file1.cpp> <file2.cpp> ...
    VARIABLE_NAMES <VAR1> <VAR2> ...
)
```

**Parameters:**
- `TARGET_NAME`: Name of the custom target to create
- `OUTPUT_SOURCES`: Variable to store generated .cpp file paths
- `SQL_FILES`: List of input SQL files (relative to project root)
- `CPP_FILES`: List of output .cpp filenames
- `VARIABLE_NAMES`: List of C++ variable names to use

### `add_sql_sources()`

Generate source and header files (for advanced usage).

```cmake
add_sql_sources(
    OUTPUT_SOURCES <sources_var>
    OUTPUT_HEADERS <headers_var>
    SQL_FILES <file1.sql> ...
    CPP_FILES <file1.cpp> ...
    VARIABLE_NAMES <VAR1> ...
)
```

### `add_sql_target()`

Create a custom target for generated files.

```cmake
add_sql_target(
    TARGET_NAME <target_name>
    SOURCES <source_list>
    HEADERS <header_list>
)
```

## Best Practices

### 1. Naming Conventions
- **SQL files**: `user_management.sql`, `product_catalog.sql`
- **CPP files**: `bkk_user_management.cpp`, `bkk_product_catalog.cpp`  
- **Variables**: `bkk_USER_MANAGEMENT_QUERY`, `bkk_PRODUCT_CATALOG_QUERY`

### 2. File Organization
```
project/
├── sql/                        # Optional: organize SQL files
│   ├── schema.sql
│   ├── queries.sql
│   └── migrations.sql
├── cmake/                      # CMake modules and utilities
│   ├── SqlGeneration.cmake     # Main module with functions
│   ├── generate_sql_header.cmake  # Internal script (auto-referenced)
│   └── README.md               # Documentation
├── examples/
│   └── CMakeLists.txt.advanced # Advanced usage patterns
└── CMakeLists.txt              # Clean project configuration
```

### 3. Adding New SQL Files
Simply add entries to the three lists:
```cmake
setup_sql_generation(
    # ... existing config ...
    SQL_FILES 
        "existing.sql"
        "new_feature.sql"      # <- Add here
    CPP_FILES
        "bkk_existing.cpp" 
        "bkk_new_feature.cpp"  # <- Add here
    VARIABLE_NAMES
        "bkk_EXISTING_QUERY"
        "bkk_NEW_FEATURE_QUERY" # <- Add here
)
```

### 4. Multiple SQL Groups
For complex projects, group related SQL files:
```cmake
# Database schema
setup_sql_generation(
    TARGET_NAME generate_schema
    OUTPUT_SOURCES SCHEMA_SOURCES
    SQL_FILES "create_tables.sql" "indexes.sql"
    CPP_FILES "bkk_tables.cpp" "bkk_indexes.cpp" 
    VARIABLE_NAMES "bkk_CREATE_TABLES" "bkk_INDEXES"
)

# Application queries  
setup_sql_generation(
    TARGET_NAME generate_queries
    OUTPUT_SOURCES QUERY_SOURCES
    SQL_FILES "user_ops.sql" "reporting.sql"
    CPP_FILES "bkk_users.cpp" "bkk_reports.cpp"
    VARIABLE_NAMES "bkk_USER_OPS" "bkk_REPORTS"
)
```

## Build Performance Benefits

### Header-only vs Source Files Comparison

| Change | Header-only Approach | Source File Approach |
|--------|---------------------|---------------------|
| SQL content changes | **All consumers recompile** | Only specific .cpp recompiles |
| Adding new consumers | Include header, rebuild all | Include header, link only |
| Large SQL files | High memory usage | Efficient memory usage |
| Build parallelization | Limited by headers | ✅ Full parallelization |

### Incremental Build Example
```bash
# Change user_queries.sql
make
# With source files: Only bkk_users.cpp recompiles + relink
# With headers: ALL files including the header recompile
```

## Error Handling

The module automatically validates:
- ✅ All SQL files exist before generation
- ✅ Parameter lists have matching lengths  
- ✅ No duplicate variable names
- ✅ Valid file paths and permissions

**Example Error:**
```
CMake Error: Required SQL file not found: /path/to/missing.sql
CMake Error: SQL_FILES and CPP_FILES must have the same number of elements
```

## Troubleshooting

### Common Issues

**1. Module not found**
```cmake
# Fix: Add module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
```

**2. Generated files not found during compilation**
```cmake
# Fix: Add build directory to includes
target_include_directories(my_app PRIVATE ${CMAKE_BINARY_DIR})
```

**3. SQL file changes not detected**
```bash
# Fix: Clean and rebuild
make clean && make
```

## Advanced Usage

See `examples/CMakeLists.txt.advanced` for complex scenarios including:
- Multiple SQL file groups
- Shared libraries with SQL data
- Migration tools and utilities
- Custom validation and testing