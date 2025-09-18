-- ========================================
-- Alternative: Create dedicated ULID functions schema
-- This approach creates a separate schema for ULID functions
-- ========================================

-- Create dedicated schema for global functions
CREATE SCHEMA IF NOT EXISTS ulid_functions 
COMMENT = 'Global ULID utility functions available to all schemas';

USE ulid_functions;

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
    
    IF CHAR_LENGTH(ulid_str) != 26 THEN
        RETURN NULL;
    END IF;
    
    WHILE i <= 26 DO
        SET c = UPPER(SUBSTRING(ulid_str, i, 1));
        
        CASE c
            WHEN '0' THEN SET val = 0;  WHEN '1' THEN SET val = 1;  WHEN '2' THEN SET val = 2;
            WHEN '3' THEN SET val = 3;  WHEN '4' THEN SET val = 4;  WHEN '5' THEN SET val = 5;
            WHEN '6' THEN SET val = 6;  WHEN '7' THEN SET val = 7;  WHEN '8' THEN SET val = 8;
            WHEN '9' THEN SET val = 9;  WHEN 'A' THEN SET val = 10; WHEN 'B' THEN SET val = 11;
            WHEN 'C' THEN SET val = 12; WHEN 'D' THEN SET val = 13; WHEN 'E' THEN SET val = 14;
            WHEN 'F' THEN SET val = 15; WHEN 'G' THEN SET val = 16; WHEN 'H' THEN SET val = 17;
            WHEN 'J' THEN SET val = 18; WHEN 'K' THEN SET val = 19; WHEN 'M' THEN SET val = 20;
            WHEN 'N' THEN SET val = 21; WHEN 'P' THEN SET val = 22; WHEN 'Q' THEN SET val = 23;
            WHEN 'R' THEN SET val = 24; WHEN 'S' THEN SET val = 25; WHEN 'T' THEN SET val = 26;
            WHEN 'V' THEN SET val = 27; WHEN 'W' THEN SET val = 28; WHEN 'X' THEN SET val = 29;
            WHEN 'Y' THEN SET val = 30; WHEN 'Z' THEN SET val = 31;
            ELSE RETURN NULL;
        END CASE;
        
        SET accumulator = (accumulator << 5) | val;
        SET bits_count = bits_count + 5;
        
        WHILE bits_count >= 8 AND byte_index < 16 DO
            SET bits_count = bits_count - 8;
            SET temp_bytes = CONCAT(temp_bytes, CHAR((accumulator >> bits_count) & 255));
            SET byte_index = byte_index + 1;
        END WHILE;
        
        SET i = i + 1;
    END WHILE;
    
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
    
    WHILE i <= 16 DO
        SET byte_val = ORD(SUBSTRING(bin_data, i, 1));
        SET accumulator = (accumulator << 8) | byte_val;
        SET bits_count = bits_count + 8;
        
        WHILE bits_count >= 5 DO
            SET bits_count = bits_count - 5;
            SET result = CONCAT(result, SUBSTRING(encoding, ((accumulator >> bits_count) & 31) + 1, 1));
        END WHILE;
        
        SET i = i + 1;
    END WHILE;
    
    IF bits_count > 0 THEN
        SET result = CONCAT(result, SUBSTRING(encoding, ((accumulator << (5 - bits_count)) & 31) + 1, 1));
    END IF;
    
    RETURN LEFT(CONCAT(result, REPEAT('0', 26)), 26);
END$$
DELIMITER ;

-- Extract timestamp from ULID
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
    
    IF CHAR_LENGTH(ulid_str) != 26 THEN
        RETURN NULL;
    END IF;
    
    SET timestamp_part = LEFT(ulid_str, 10);
    
    WHILE i <= 10 DO
        SET c = UPPER(SUBSTRING(timestamp_part, i, 1));
        
        CASE c
            WHEN '0' THEN SET val = 0;  WHEN '1' THEN SET val = 1;  WHEN '2' THEN SET val = 2;
            WHEN '3' THEN SET val = 3;  WHEN '4' THEN SET val = 4;  WHEN '5' THEN SET val = 5;
            WHEN '6' THEN SET val = 6;  WHEN '7' THEN SET val = 7;  WHEN '8' THEN SET val = 8;
            WHEN '9' THEN SET val = 9;  WHEN 'A' THEN SET val = 10; WHEN 'B' THEN SET val = 11;
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

-- Grant access to all users for the ULID functions schema
GRANT USAGE ON ulid_functions.* TO '%'@'%';
GRANT EXECUTE ON FUNCTION ulid_functions.ULID_TO_BIN TO '%'@'%';
GRANT EXECUTE ON FUNCTION ulid_functions.BIN_TO_ULID TO '%'@'%';
GRANT EXECUTE ON FUNCTION ulid_functions.ULID_TIMESTAMP TO '%'@'%';

-- Create convenience views/procedures for easier access
-- (Optional: create synonyms or wrapper functions in each schema)

-- Usage examples from any schema:
SELECT 'Examples using ulid_functions schema:' as info;
SELECT 'ulid_functions.ULID_TO_BIN("01FJYWZ3RJM927XKDJGDR06REA")' as example1;
SELECT 'ulid_functions.BIN_TO_ULID(binary_data)' as example2;
SELECT 'ulid_functions.ULID_TIMESTAMP("01FJYWZ3RJM927XKDJGDR06REA")' as example3;