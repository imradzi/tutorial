#!/bin/bash

# ========================================
# Setup Global ULID Functions in MySQL
# ========================================

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default connection parameters
MYSQL_HOST=${MYSQL_HOST:-"127.0.0.1"}
MYSQL_PORT=${MYSQL_PORT:-"33061"}
MYSQL_USER=${MYSQL_USER:-"root"}
MYSQL_PASSWORD=${MYSQL_PASSWORD:-"ft1832h"}
MYSQL_PROTOCOL=${MYSQL_PROTOCOL:-"mysqlx"}

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Setting up Global ULID Functions${NC}"
echo -e "${BLUE}========================================${NC}"

# Function to check if MySQL is accessible
check_mysql_connection() {
    echo -e "${YELLOW}Testing MySQL connection...${NC}"
    
    if command -v mysql >/dev/null 2>&1; then
        # Try traditional MySQL connection
        mysql -h "$MYSQL_HOST" -P 3306 -u "$MYSQL_USER" -p"$MYSQL_PASSWORD" -e "SELECT VERSION();" >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✅ MySQL connection successful${NC}"
            return 0
        fi
    fi
    
    echo -e "${RED}❌ Cannot connect to MySQL${NC}"
    echo -e "${YELLOW}Please check your connection parameters:${NC}"
    echo "  Host: $MYSQL_HOST"
    echo "  Port: $MYSQL_PORT"
    echo "  User: $MYSQL_USER"
    return 1
}

# Function to install functions in mysql system database
install_system_functions() {
    echo -e "${YELLOW}Installing ULID functions in mysql system database...${NC}"
    
    mysql -h "$MYSQL_HOST" -P 3306 -u "$MYSQL_USER" -p"$MYSQL_PASSWORD" < mysql_global_ulid_functions.sql
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✅ System functions installed successfully${NC}"
        return 0
    else
        echo -e "${RED}❌ Failed to install system functions${NC}"
        return 1
    fi
}

# Function to install functions in separate schema
install_schema_functions() {
    echo -e "${YELLOW}Installing ULID functions in dedicated schema...${NC}"
    
    mysql -h "$MYSQL_HOST" -P 3306 -u "$MYSQL_USER" -p"$MYSQL_PASSWORD" < mysql_shared_ulid_schema.sql
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✅ Schema functions installed successfully${NC}"
        return 0
    else
        echo -e "${RED}❌ Failed to install schema functions${NC}"
        return 1
    fi
}

# Function to test the installed functions
test_functions() {
    echo -e "${YELLOW}Testing installed functions...${NC}"
    
    # Test mysql schema functions
    echo "Testing mysql.ULID_TO_BIN()..."
    mysql -h "$MYSQL_HOST" -P 3306 -u "$MYSQL_USER" -p"$MYSQL_PASSWORD" -e "SELECT HEX(mysql.ULID_TO_BIN('01FJYWZ3RJM927XKDJGDR06REA')) as binary_hex;"
    
    echo "Testing mysql.BIN_TO_ULID()..."
    mysql -h "$MYSQL_HOST" -P 3306 -u "$MYSQL_USER" -p"$MYSQL_PASSWORD" -e "SELECT mysql.BIN_TO_ULID(mysql.ULID_TO_BIN('01FJYWZ3RJM927XKDJGDR06REA')) as ulid_roundtrip;"
    
    echo "Testing mysql.ULID_TIMESTAMP()..."
    mysql -h "$MYSQL_HOST" -P 3306 -u "$MYSQL_USER" -p"$MYSQL_PASSWORD" -e "SELECT mysql.ULID_TIMESTAMP('01FJYWZ3RJM927XKDJGDR06REA') as timestamp_ms;"
    
    echo "Testing mysql.ULID_TO_DATETIME()..."
    mysql -h "$MYSQL_HOST" -P 3306 -u "$MYSQL_USER" -p"$MYSQL_PASSWORD" -e "SELECT mysql.ULID_TO_DATETIME('01FJYWZ3RJM927XKDJGDR06REA') as datetime_val;"
}

# Function to show usage examples
show_usage_examples() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}Usage Examples${NC}"
    echo -e "${BLUE}========================================${NC}"
    
    cat << 'EOF'

-- Using global functions from any schema:

-- 1. Convert ULID string to binary for storage
SELECT mysql.ULID_TO_BIN('01FJYWZ3RJM927XKDJGDR06REA');

-- 2. Convert binary back to ULID string
SELECT mysql.BIN_TO_ULID(binary_column) FROM your_table;

-- 3. Extract timestamp from ULID
SELECT mysql.ULID_TIMESTAMP('01FJYWZ3RJM927XKDJGDR06REA');

-- 4. Convert ULID to MySQL DATETIME
SELECT mysql.ULID_TO_DATETIME('01FJYWZ3RJM927XKDJGDR06REA');

-- 5. Create table with BINARY(16) ULID column
CREATE TABLE users (
    id BINARY(16) PRIMARY KEY,
    email VARCHAR(255) NOT NULL,
    -- Use mysql.BIN_TO_ULID(id) to display ULID as string
    INDEX idx_email (email)
);

-- 6. Insert using C++ ULID class (binary data directly)
-- INSERT INTO users (id, email) VALUES (?, ?)
-- where first ? is bound to ULID.data() with size 16

-- 7. Query with conversion for display
SELECT mysql.BIN_TO_ULID(id) as ulid_string, email 
FROM users 
ORDER BY id;  -- Natural chronological order

-- 8. Range queries by timestamp
SELECT mysql.BIN_TO_ULID(id) as ulid_string, email
FROM users 
WHERE id >= mysql.ULID_TO_BIN('01FJYWZ0000000000000000000')  -- Start of timerange
  AND id <  mysql.ULID_TO_BIN('01FJYWZZZZZZZZZZZZZZZZZZZZ')   -- End of timerange
ORDER BY id;

EOF
}

# Main execution
main() {
    # Check if required files exist
    if [ ! -f "mysql_global_ulid_functions.sql" ]; then
        echo -e "${RED}❌ mysql_global_ulid_functions.sql not found${NC}"
        exit 1
    fi
    
    # Check MySQL connection
    check_mysql_connection
    if [ $? -ne 0 ]; then
        echo -e "${RED}Please verify MySQL is running and credentials are correct${NC}"
        exit 1
    fi
    
    # Install functions
    echo -e "${YELLOW}Choose installation method:${NC}"
    echo "1. Install in mysql system database (recommended)"
    echo "2. Install in separate ulid_functions schema"
    echo "3. Install both"
    read -p "Enter choice (1-3): " choice
    
    case $choice in
        1)
            install_system_functions
            ;;
        2)
            install_schema_functions
            ;;
        3)
            install_system_functions
            install_schema_functions
            ;;
        *)
            echo -e "${RED}Invalid choice${NC}"
            exit 1
            ;;
    esac
    
    # Test the functions
    echo ""
    read -p "Test the installed functions? (y/n): " test_choice
    if [[ $test_choice =~ ^[Yy]$ ]]; then
        test_functions
    fi
    
    # Show usage examples
    show_usage_examples
    
    echo -e "${GREEN}✅ Setup complete!${NC}"
    echo -e "${BLUE}The ULID functions are now available globally across all schemas.${NC}"
}

# Show help
if [[ $1 == "--help" || $1 == "-h" ]]; then
    echo "Setup Global ULID Functions for MySQL"
    echo ""
    echo "Usage: $0 [options]"
    echo ""
    echo "Environment variables:"
    echo "  MYSQL_HOST     MySQL host (default: 127.0.0.1)"
    echo "  MYSQL_PORT     MySQL port (default: 33061)"
    echo "  MYSQL_USER     MySQL user (default: root)"
    echo "  MYSQL_PASSWORD MySQL password (default: ft1832h)"
    echo ""
    echo "Example:"
    echo "  MYSQL_HOST=localhost MYSQL_PORT=3306 $0"
    exit 0
fi

# Run main function
main "$@"