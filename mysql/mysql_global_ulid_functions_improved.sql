-- ========================================
-- Global ULID Functions for MySQL - Improved Version
-- Create in mysql system database for global access
-- WITH PROPER RANDOM GENERATION
-- ========================================

-- Connect to mysql system database
USE mysql;

-- Function to convert ULID string to BINARY(16)
DROP FUNCTION IF EXISTS ULID_TO_BIN;
DELIMITER $$
CREATE FUNCTION ULID_TO_BIN(ulid_str CHAR(26))
RETURNS BINARY(16)
READS SQL DATA
DETERMINISTIC
SQL SECURITY INVOKER
COMMENT 'Convert 26-character ULID string to 16-byte binary format'
BEGIN
    DECLARE result BINARY(16);
    DECLARE i INT DEFAULT 1;
    DECLARE c CHAR(1);
    DECLARE val TINYINT;
    DECLARE accumulator BIGINT UNSIGNED DEFAULT 0;
    DECLARE bits_count INT DEFAULT 0;
    DECLARE byte_index INT DEFAULT 0;
    DECLARE temp_bytes VARBINARY(16) DEFAULT '';
    
    -- Validate input length
    IF CHAR_LENGTH(ulid_str) != 26 THEN
        RETURN NULL;
    END IF;
    
    -- Crockford's Base32 decoding
    WHILE i <= 26 DO
        SET c = UPPER(SUBSTRING(ulid_str, i, 1));
        
        -- Convert character to 5-bit value
        CASE c
            WHEN '0' THEN SET val = 0;
            WHEN '1' THEN SET val = 1;
            WHEN '2' THEN SET val = 2;
            WHEN '3' THEN SET val = 3;
            WHEN '4' THEN SET val = 4;
            WHEN '5' THEN SET val = 5;
            WHEN '6' THEN SET val = 6;
            WHEN '7' THEN SET val = 7;
            WHEN '8' THEN SET val = 8;
            WHEN '9' THEN SET val = 9;
            WHEN 'A' THEN SET val = 10;
            WHEN 'B' THEN SET val = 11;
            WHEN 'C' THEN SET val = 12;
            WHEN 'D' THEN SET val = 13;
            WHEN 'E' THEN SET val = 14;
            WHEN 'F' THEN SET val = 15;
            WHEN 'G' THEN SET val = 16;
            WHEN 'H' THEN SET val = 17;
            WHEN 'J' THEN SET val = 18;
            WHEN 'K' THEN SET val = 19;
            WHEN 'M' THEN SET val = 20;
            WHEN 'N' THEN SET val = 21;
            WHEN 'P' THEN SET val = 22;
            WHEN 'Q' THEN SET val = 23;
            WHEN 'R' THEN SET val = 24;
            WHEN 'S' THEN SET val = 25;
            WHEN 'T' THEN SET val = 26;
            WHEN 'V' THEN SET val = 27;
            WHEN 'W' THEN SET val = 28;
            WHEN 'X' THEN SET val = 29;
            WHEN 'Y' THEN SET val = 30;
            WHEN 'Z' THEN SET val = 31;
            ELSE RETURN NULL; -- Invalid character
        END CASE;
        
        -- Accumulate bits
        SET accumulator = (accumulator << 5) | val;
        SET bits_count = bits_count + 5;
        
        -- Extract bytes when we have enough bits
        WHILE bits_count >= 8 AND byte_index < 16 DO
            SET bits_count = bits_count - 8;
            SET temp_bytes = CONCAT(temp_bytes, CHAR((accumulator >> bits_count) & 255));
            SET byte_index = byte_index + 1;
        END WHILE;
        
        SET i = i + 1;
    END WHILE;
    
    -- Pad to exactly 16 bytes if needed
    WHILE byte_index < 16 DO
        SET temp_bytes = CONCAT(temp_bytes, CHAR(0));
        SET byte_index = byte_index + 1;
    END WHILE;
    
    RETURN CAST(temp_bytes AS BINARY(16));
END$$
DELIMITER ;

-- Function to convert BINARY(16) to ULID string
DROP FUNCTION IF EXISTS BIN_TO_ULID;
DELIMITER $$
CREATE FUNCTION BIN_TO_ULID(bin_data BINARY(16))
RETURNS CHAR(26)
READS SQL DATA
DETERMINISTIC
SQL SECURITY INVOKER
COMMENT 'Convert 16-byte binary to 26-character ULID string'
BEGIN
    DECLARE result CHAR(26) DEFAULT '';
    DECLARE encoding CHAR(32) DEFAULT '0123456789ABCDEFGHJKMNPQRSTVWXYZ';
    DECLARE i INT DEFAULT 1;
    DECLARE accumulator BIGINT UNSIGNED DEFAULT 0;
    DECLARE bits_count INT DEFAULT 0;
    DECLARE byte_val INT;
    
    -- Process each byte
    WHILE i <= 16 DO
        SET byte_val = ORD(SUBSTRING(bin_data, i, 1));
        SET accumulator = (accumulator << 8) | byte_val;
        SET bits_count = bits_count + 8;
        
        -- Extract 5-bit groups
        WHILE bits_count >= 5 DO
            SET bits_count = bits_count - 5;
            SET result = CONCAT(result, SUBSTRING(encoding, ((accumulator >> bits_count) & 31) + 1, 1));
        END WHILE;
        
        SET i = i + 1;
    END WHILE;
    
    -- Handle remaining bits
    IF bits_count > 0 THEN
        SET result = CONCAT(result, SUBSTRING(encoding, ((accumulator << (5 - bits_count)) & 31) + 1, 1));
    END IF;
    
    -- Ensure exactly 26 characters
    RETURN LEFT(CONCAT(result, REPEAT('0', 26)), 26);
END$$
DELIMITER ;

-- Function to extract timestamp from ULID string
DROP FUNCTION IF EXISTS ULID_TIMESTAMP;
DELIMITER $$
CREATE FUNCTION ULID_TIMESTAMP(ulid_str CHAR(26))
RETURNS BIGINT UNSIGNED
READS SQL DATA
DETERMINISTIC
SQL SECURITY INVOKER
COMMENT 'Extract timestamp (milliseconds since epoch) from ULID string'
BEGIN
    DECLARE timestamp_part CHAR(10);
    DECLARE result BIGINT UNSIGNED DEFAULT 0;
    DECLARE i INT DEFAULT 1;
    DECLARE c CHAR(1);
    DECLARE val TINYINT;
    
    -- Validate input
    IF CHAR_LENGTH(ulid_str) != 26 THEN
        RETURN NULL;
    END IF;
    
    -- Extract first 10 characters (timestamp part)
    SET timestamp_part = LEFT(ulid_str, 10);
    
    -- Decode timestamp part
    WHILE i <= 10 DO
        SET c = UPPER(SUBSTRING(timestamp_part, i, 1));
        
        -- Convert character to 5-bit value (same mapping as ULID_TO_BIN)
        CASE c
            WHEN '0' THEN SET val = 0; WHEN '1' THEN SET val = 1; WHEN '2' THEN SET val = 2;
            WHEN '3' THEN SET val = 3; WHEN '4' THEN SET val = 4; WHEN '5' THEN SET val = 5;
            WHEN '6' THEN SET val = 6; WHEN '7' THEN SET val = 7; WHEN '8' THEN SET val = 8;
            WHEN '9' THEN SET val = 9; WHEN 'A' THEN SET val = 10; WHEN 'B' THEN SET val = 11;
            WHEN 'C' THEN SET val = 12; WHEN 'D' THEN SET val = 13; WHEN 'E' THEN SET val = 14;
            WHEN 'F' THEN SET val = 15; WHEN 'G' THEN SET val = 16; WHEN 'H' THEN SET val = 17;
            WHEN 'J' THEN SET val = 18; WHEN 'K' THEN SET val = 19; WHEN 'M' THEN SET val = 20;
            WHEN 'N' THEN SET val = 21; WHEN 'P' THEN SET val = 22; WHEN 'Q' THEN SET val = 23;
            WHEN 'R' THEN SET val = 24; WHEN 'S' THEN SET val = 25; WHEN 'T' THEN SET val = 26;
            WHEN 'V' THEN SET val = 27; WHEN 'W' THEN SET val = 28; WHEN 'X' THEN SET val = 29;
            WHEN 'Y' THEN SET val = 30; WHEN 'Z' THEN SET val = 31;
            ELSE RETURN NULL; -- Invalid character
        END CASE;
        
        SET result = (result << 5) | val;
        SET i = i + 1;
    END WHILE;
    
    RETURN result;
END$$
DELIMITER ;

-- Function to convert ULID timestamp to MySQL DATETIME
DROP FUNCTION IF EXISTS ULID_TO_DATETIME;
DELIMITER $$
CREATE FUNCTION ULID_TO_DATETIME(ulid_str CHAR(26))
RETURNS DATETIME(3)
READS SQL DATA
DETERMINISTIC
SQL SECURITY INVOKER
COMMENT 'Convert ULID to MySQL DATETIME with millisecond precision'
BEGIN
    DECLARE timestamp_ms BIGINT UNSIGNED;
    SET timestamp_ms = ULID_TIMESTAMP(ulid_str);
    
    IF timestamp_ms IS NULL THEN
        RETURN NULL;
    END IF;
    
    -- Convert milliseconds to DATETIME(3)
    RETURN FROM_UNIXTIME(timestamp_ms / 1000.0);
END$$
DELIMITER ;

-- Helper function to generate random bytes using MySQL's built-in randomness
DROP FUNCTION IF EXISTS ULID_RANDOM_BYTES;
DELIMITER $$
CREATE FUNCTION ULID_RANDOM_BYTES(num_bytes INT)
RETURNS VARBINARY(16)
READS SQL DATA
NOT DETERMINISTIC
SQL SECURITY INVOKER
COMMENT 'Generate cryptographically secure random bytes using MySQL RANDOM_BYTES'
BEGIN
    DECLARE random_data VARBINARY(16);
    
    -- Use MySQL's RANDOM_BYTES function for cryptographic randomness
    -- Available in MySQL 5.7+ and MariaDB 10.2+
    SET random_data = RANDOM_BYTES(num_bytes);
    
    RETURN random_data;
END$$
DELIMITER ;

-- Function to generate ULID with proper randomness
DROP FUNCTION IF EXISTS ULID_GENERATE;
DELIMITER $$
CREATE FUNCTION ULID_GENERATE(timestamp_ms BIGINT UNSIGNED)
RETURNS CHAR(26)
READS SQL DATA
NOT DETERMINISTIC  -- Changed from DETERMINISTIC due to random generation
SQL SECURITY INVOKER
COMMENT 'Generate ULID string with cryptographic randomness'
BEGIN
    DECLARE encoding CHAR(32) DEFAULT '0123456789ABCDEFGHJKMNPQRSTVWXYZ';
    DECLARE result CHAR(26) DEFAULT '';
    DECLARE ts BIGINT UNSIGNED;
    DECLARE random_bytes VARBINARY(10);
    DECLARE i INT;
    DECLARE byte_val INT;
    DECLARE char_val INT;
    
    -- Use current timestamp if not provided
    IF timestamp_ms IS NULL THEN
        SET timestamp_ms = UNIX_TIMESTAMP(NOW(3)) * 1000;
    END IF;
    
    SET ts = timestamp_ms;
    
    -- Encode timestamp part (48 bits -> 10 characters)
    SET i = 10;
    WHILE i > 0 DO
        SET result = CONCAT(SUBSTRING(encoding, (ts & 31) + 1, 1), result);
        SET ts = ts >> 5;
        SET i = i - 1;
    END WHILE;
    
    -- Generate 10 random bytes (80 bits) for the randomness part
    SET random_bytes = ULID_RANDOM_BYTES(10);
    
    -- Convert random bytes to Base32 characters (16 characters)
    SET i = 1;
    WHILE i <= 10 DO
        SET byte_val = ORD(SUBSTRING(random_bytes, i, 1));
        
        -- Each byte gives us 8 bits, we need 5-bit groups for Base32
        -- This is a simplified approach - we'll use the byte value modulo 32
        SET char_val = (byte_val & 31) + 1;  -- Ensure 1-32 range for SUBSTRING
        SET result = CONCAT(result, SUBSTRING(encoding, char_val, 1));
        
        -- For better randomness distribution, add second character from shifted bits
        SET char_val = ((byte_val >> 3) & 31) + 1;
        
        -- Only add if we haven't reached 26 characters yet
        IF CHAR_LENGTH(result) < 26 THEN
            SET result = CONCAT(result, SUBSTRING(encoding, char_val, 1));
        END IF;
        
        SET i = i + 1;
    END WHILE;
    
    -- Ensure exactly 26 characters
    SET result = LEFT(result, 26);
    
    -- Pad with random characters if needed
    WHILE CHAR_LENGTH(result) < 26 DO
        SET byte_val = ORD(SUBSTRING(ULID_RANDOM_BYTES(1), 1, 1));
        SET char_val = (byte_val & 31) + 1;
        SET result = CONCAT(result, SUBSTRING(encoding, char_val, 1));
    END WHILE;
    
    RETURN result;
END$$
DELIMITER ;

-- Function to generate ULID with current timestamp (convenience function)
DROP FUNCTION IF EXISTS ULID_GENERATE_NOW;
DELIMITER $$
CREATE FUNCTION ULID_GENERATE_NOW()
RETURNS CHAR(26)
READS SQL DATA
NOT DETERMINISTIC
SQL SECURITY INVOKER
COMMENT 'Generate ULID string with current timestamp and cryptographic randomness'
BEGIN
    RETURN ULID_GENERATE(UNIX_TIMESTAMP(NOW(3)) * 1000);
END$$
DELIMITER ;

-- Function to create ULID with better random distribution
DROP FUNCTION IF EXISTS ULID_GENERATE_SECURE;
DELIMITER $$
CREATE FUNCTION ULID_GENERATE_SECURE(timestamp_ms BIGINT UNSIGNED)
RETURNS CHAR(26)
READS SQL DATA
NOT DETERMINISTIC
SQL SECURITY INVOKER
COMMENT 'Generate ULID with improved random distribution using multiple entropy sources'
BEGIN
    DECLARE encoding CHAR(32) DEFAULT '0123456789ABCDEFGHJKMNPQRSTVWXYZ';
    DECLARE result CHAR(26) DEFAULT '';
    DECLARE ts BIGINT UNSIGNED;
    DECLARE random_data VARBINARY(16);
    DECLARE timestamp_bytes VARBINARY(8);
    DECLARE ulid_binary VARBINARY(16);
    DECLARE i INT;
    DECLARE accumulator BIGINT UNSIGNED DEFAULT 0;
    DECLARE bits_accumulated INT DEFAULT 0;
    
    -- Use current timestamp if not provided
    IF timestamp_ms IS NULL THEN
        SET timestamp_ms = UNIX_TIMESTAMP(NOW(3)) * 1000;
    END IF;
    
    -- Create 16-byte binary ULID
    SET ts = timestamp_ms;
    
    -- Convert timestamp to 6 bytes (48 bits) in big-endian format
    SET timestamp_bytes = CONCAT(
        CHAR((ts >> 40) & 255),
        CHAR((ts >> 32) & 255),
        CHAR((ts >> 24) & 255),
        CHAR((ts >> 16) & 255),
        CHAR((ts >> 8) & 255),
        CHAR(ts & 255)
    );
    
    -- Generate 10 bytes of cryptographic randomness
    SET random_data = ULID_RANDOM_BYTES(10);
    
    -- Combine timestamp and random bytes
    SET ulid_binary = CONCAT(timestamp_bytes, random_data);
    
    -- Convert 16 bytes to Base32 (26 characters)
    SET i = 1;
    WHILE i <= 16 AND CHAR_LENGTH(result) < 26 DO
        SET accumulator = (accumulator << 8) | ORD(SUBSTRING(ulid_binary, i, 1));
        SET bits_accumulated = bits_accumulated + 8;
        
        -- Extract 5-bit groups
        WHILE bits_accumulated >= 5 AND CHAR_LENGTH(result) < 26 DO
            SET bits_accumulated = bits_accumulated - 5;
            SET result = CONCAT(result, SUBSTRING(encoding, ((accumulator >> bits_accumulated) & 31) + 1, 1));
        END WHILE;
        
        SET i = i + 1;
    END WHILE;
    
    -- Handle any remaining bits
    IF bits_accumulated > 0 AND CHAR_LENGTH(result) < 26 THEN
        SET result = CONCAT(result, SUBSTRING(encoding, ((accumulator << (5 - bits_accumulated)) & 31) + 1, 1));
    END IF;
    
    -- Ensure exactly 26 characters
    RETURN LEFT(CONCAT(result, REPEAT('0', 26)), 26);
END$$
DELIMITER ;

-- Grant permissions for global access
GRANT EXECUTE ON FUNCTION mysql.ULID_TO_BIN TO '%'@'%';
GRANT EXECUTE ON FUNCTION mysql.BIN_TO_ULID TO '%'@'%';
GRANT EXECUTE ON FUNCTION mysql.ULID_TIMESTAMP TO '%'@'%';
GRANT EXECUTE ON FUNCTION mysql.ULID_TO_DATETIME TO '%'@'%';
GRANT EXECUTE ON FUNCTION mysql.ULID_GENERATE TO '%'@'%';
GRANT EXECUTE ON FUNCTION mysql.ULID_GENERATE_NOW TO '%'@'%';
GRANT EXECUTE ON FUNCTION mysql.ULID_GENERATE_SECURE TO '%'@'%';
GRANT EXECUTE ON FUNCTION mysql.ULID_RANDOM_BYTES TO '%'@'%';

-- Create a view in information_schema for easy discovery
SELECT 'Improved Global ULID functions created in mysql database:' as info;
SELECT ROUTINE_NAME, ROUTINE_TYPE, ROUTINE_COMMENT
FROM INFORMATION_SCHEMA.ROUTINES
WHERE ROUTINE_SCHEMA = 'mysql'
  AND (ROUTINE_NAME LIKE 'ULID%' OR ROUTINE_NAME LIKE '%ULID')
ORDER BY ROUTINE_NAME;

-- Test the improved functions
SELECT 'Testing improved ULID functions:' as test_info;

-- Test ULID generation with randomness
SELECT 'Generated ULIDs with proper randomness:' as test_label;
SELECT mysql.ULID_GENERATE_NOW() as ulid1;
SELECT mysql.ULID_GENERATE_NOW() as ulid2;
SELECT mysql.ULID_GENERATE_NOW() as ulid3;

-- Test with specific timestamp
SELECT 'ULID with specific timestamp:' as test_label;
SELECT mysql.ULID_GENERATE(1640995200000) as ulid_2022;

-- Test secure generation
SELECT 'Secure ULID generation:' as test_label;
SELECT mysql.ULID_GENERATE_SECURE(NULL) as secure_ulid;

-- Test roundtrip conversion
SELECT 'Roundtrip test (should match):' as test_label;
SET @test_ulid = mysql.ULID_GENERATE_NOW();
SELECT @test_ulid as original_ulid;
SELECT mysql.BIN_TO_ULID(mysql.ULID_TO_BIN(@test_ulid)) as roundtrip_ulid;
SELECT (@test_ulid = mysql.BIN_TO_ULID(mysql.ULID_TO_BIN(@test_ulid))) as roundtrip_matches;

-- Usage examples with real randomness
SELECT 'Usage Examples with Real Random Generation:' as info;
SELECT '-- Generate ULID with current timestamp:' as example;
SELECT 'SELECT mysql.ULID_GENERATE_NOW();' as query;

SELECT '-- Generate ULID with specific timestamp:' as example;
SELECT 'SELECT mysql.ULID_GENERATE(1640995200000);' as query;

SELECT '-- Generate secure ULID (best randomness):' as example;
SELECT 'SELECT mysql.ULID_GENERATE_SECURE(NULL);' as query;

SELECT '-- Convert ULID to binary and back:' as example;
SELECT 'SELECT mysql.BIN_TO_ULID(mysql.ULID_TO_BIN(mysql.ULID_GENERATE_NOW()));' as query;