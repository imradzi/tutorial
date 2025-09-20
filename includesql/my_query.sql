-- ========================================
-- Fixed Global ULID Functions for MySQL 
-- Fixes the "Data too long for column" error
-- ========================================

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
            ELSE RETURN NULL;
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
    DECLARE result VARCHAR(30) DEFAULT '';  -- Increased size to prevent overflow
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
    
    -- Ensure exactly 26 characters and return as CHAR(26)
    RETURN CAST(LEFT(CONCAT(result, REPEAT('0', 26)), 26) AS CHAR(26));
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
            ELSE RETURN NULL;
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
    
    RETURN FROM_UNIXTIME(timestamp_ms / 1000.0);
END$$
DELIMITER ;

-- Enhanced version using RANDOM_BYTES if available
DROP FUNCTION IF EXISTS ULID_GENERATE;
DELIMITER $$
CREATE FUNCTION ULID_GENERATE(timestamp_ms BIGINT UNSIGNED)
RETURNS CHAR(26)
READS SQL DATA
NOT DETERMINISTIC
SQL SECURITY INVOKER
COMMENT 'Generate ULID using RANDOM_BYTES for better entropy'
BEGIN
    DECLARE encoding CHAR(32) DEFAULT '0123456789ABCDEFGHJKMNPQRSTVWXYZ';
    DECLARE result VARCHAR(30) DEFAULT '';
    DECLARE ts BIGINT UNSIGNED;
    DECLARE random_bytes VARBINARY(10);
    DECLARE i INT;
    DECLARE byte_val INT;
    
    -- Use current timestamp if not provided
    IF timestamp_ms IS NULL THEN
        SET timestamp_ms = UNIX_TIMESTAMP(NOW(3)) * 1000;
    END IF;
    
    SET ts = timestamp_ms;
    
    -- Build timestamp part (10 characters)
    SET i = 0;
    WHILE i < 10 DO
        SET result = CONCAT(SUBSTRING(encoding, (ts & 31) + 1, 1), result);
        SET ts = ts >> 5;
        SET i = i + 1;
    END WHILE;
    
    -- Generate random bytes (try RANDOM_BYTES, fallback to RAND)
    BEGIN
        DECLARE CONTINUE HANDLER FOR SQLEXCEPTION
        BEGIN
            -- Fallback to RAND() if RANDOM_BYTES not available
            SET i = 0;
            WHILE i < 16 DO
                SET result = CONCAT(result, SUBSTRING(encoding, FLOOR(RAND() * 32) + 1, 1));
                SET i = i + 1;
            END WHILE;
        END;
        
        -- Try to use RANDOM_BYTES
        SET random_bytes = RANDOM_BYTES(10);
        SET i = 1;
        WHILE i <= 10 AND CHAR_LENGTH(result) < 26 DO
            SET byte_val = ORD(SUBSTRING(random_bytes, i, 1));
            -- Convert each byte to Base32 characters
            SET result = CONCAT(result, SUBSTRING(encoding, (byte_val & 31) + 1, 1));
            IF CHAR_LENGTH(result) < 26 THEN
                SET result = CONCAT(result, SUBSTRING(encoding, ((byte_val >> 5) & 7) + 1, 1));
            END IF;
            SET i = i + 1;
        END WHILE;
    END;
    
    -- Ensure exactly 26 characters
    RETURN CAST(LEFT(CONCAT(result, REPEAT('0', 26)), 26) AS CHAR(26));
END$$
DELIMITER ;

-- Convenience function for current timestamp
DROP FUNCTION IF EXISTS ULID_GENERATE_NOW;
DELIMITER $$
CREATE FUNCTION ULID_GENERATE_NOW()
RETURNS CHAR(26)
READS SQL DATA
NOT DETERMINISTIC
SQL SECURITY INVOKER
COMMENT 'Generate ULID string with current timestamp and randomness'
BEGIN
    RETURN ULID_GENERATE(NULL);
END$$
DELIMITER ;

-- Convenience function for current timestamp
DROP FUNCTION IF EXISTS ULID;
DELIMITER $$
CREATE FUNCTION ULID()
RETURNS BINARY(16)
READS SQL DATA
NOT DETERMINISTIC
SQL SECURITY INVOKER
COMMENT 'Generate ULID string with current timestamp and randomness'
BEGIN
    RETURN ULID_TO_BIN(ULID_GENERATE(NULL));
END$$
DELIMITER ;



-- Grant permissions
GRANT EXECUTE ON FUNCTION mysql.ULID_TO_BIN TO '%'@'%';
GRANT EXECUTE ON FUNCTION mysql.BIN_TO_ULID TO '%'@'%';
GRANT EXECUTE ON FUNCTION mysql.ULID_TIMESTAMP TO '%'@'%';
GRANT EXECUTE ON FUNCTION mysql.ULID_TO_DATETIME TO '%'@'%';
GRANT EXECUTE ON FUNCTION mysql.ULID_GENERATE TO '%'@'%';
GRANT EXECUTE ON FUNCTION mysql.ULID_GENERATE_NOW TO '%'@'%';
GRANT EXECUTE ON FUNCTION mysql.ULID TO '%'@'%';

-- Test the fixed functions
SELECT 'Testing fixed ULID functions:' as test_info;

SELECT mysql.ULID_GENERATE_NOW() as generated_ulid_1;
SELECT mysql.ULID_GENERATE_NOW() as generated_ulid_2;
SELECT mysql.ULID_GENERATE_NOW() as generated_ulid_3;

-- Test with specific timestamp
SELECT mysql.ULID_GENERATE(1640995200000) as ulid_2022;

-- Test roundtrip
SET @test_ulid = mysql.ULID_GENERATE_NOW();
SELECT @test_ulid as original;
SELECT mysql.BIN_TO_ULID(mysql.ULID_TO_BIN(@test_ulid)) as roundtrip;
SELECT (@test_ulid = mysql.BIN_TO_ULID(mysql.ULID_TO_BIN(@test_ulid))) as success;

SELECT 'All functions should work without "Data too long" errors now!' as result;