-- MySQL User-Defined Functions for ULID binary-string conversion

-- Function to convert binary ULID to string representation
DELIMITER //
CREATE FUNCTION BIN_TO_ULID(bin_ulid BINARY(16)) RETURNS CHAR(26)
DETERMINISTIC
BEGIN
    DECLARE result CHAR(26);
    DECLARE i INT DEFAULT 0;
    DECLARE part BIGINT UNSIGNED;
    DECLARE encoding CHAR(32) DEFAULT '0123456789ABCDEFGHJKMNPQRSTVWXYZ';
    
    -- Initialize result with placeholder characters
    SET result = '00000000000000000000000000';
    
    -- Process timestamp part (first 6 bytes, 48 bits)
    SET part = CONV(HEX(SUBSTRING(bin_ulid, 1, 6)), 16, 10);
    SET i = 10;
    WHILE i > 0 DO
        SET result = INSERT(result, i, 1, SUBSTRING(encoding, (part % 32) + 1, 1));
        SET part = part DIV 32;
        SET i = i - 1;
    END WHILE;
    
    -- Process random part (next 10 bytes, 80 bits) in two segments
    -- First segment (5 bytes, 40 bits)
    SET part = CONV(HEX(SUBSTRING(bin_ulid, 7, 5)), 16, 10);
    SET i = 18;
    WHILE i > 10 DO
        SET result = INSERT(result, i, 1, SUBSTRING(encoding, (part % 32) + 1, 1));
        SET part = part DIV 32;
        SET i = i - 1;
    END WHILE;
    
    -- Second segment (5 bytes, 40 bits)
    SET part = CONV(HEX(SUBSTRING(bin_ulid, 12, 5)), 16, 10);
    SET i = 26;
    WHILE i > 18 DO
        SET result = INSERT(result, i, 1, SUBSTRING(encoding, (part % 32) + 1, 1));
        SET part = part DIV 32;
        SET i = i - 1;
    END WHILE;
    
    RETURN result;
END //
DELIMITER ;

-- Function to convert string ULID to binary representation
DELIMITER //
CREATE FUNCTION ULID_TO_BIN(str_ulid CHAR(26)) RETURNS BINARY(16)
DETERMINISTIC
BEGIN
    DECLARE result BINARY(16);
    DECLARE i INT DEFAULT 1;
    DECLARE c CHAR(1);
    DECLARE val INT;
    DECLARE decoding VARCHAR(256) DEFAULT 
        '                                '
        '                0123456789      '
        '    ABCDEFGH JKLMN PQRST VWXYZ '
        '    abcdefgh jklmn pqrst vwxyz ';
    
    -- Initialize an empty binary result
    SET result = UNHEX(REPEAT('00', 16));
    
    -- Extract timestamp part (first 10 characters)
    SET i = 1;
    WHILE i <= 10 DO
        SET c = SUBSTRING(str_ulid, i, 1);
        SET val = ORD(SUBSTRING(decoding, ASCII(c), 1)) - 48; -- Convert from ASCII to value
        
        IF val < 0 OR val > 31 THEN
            RETURN NULL; -- Invalid character
        END IF;
        
        -- Combine into bytes
        SET result = UNHEX(
            CONCAT(
                HEX(result),
                LPAD(HEX(val), 2, '0')
            )
        );
        
        SET i = i + 1;
    END WHILE;
    
    -- Extract randomness part (next 16 characters)
    WHILE i <= 26 DO
        SET c = SUBSTRING(str_ulid, i, 1);
        SET val = ORD(SUBSTRING(decoding, ASCII(c), 1)) - 48;
        
        IF val < 0 OR val > 31 THEN
            RETURN NULL; -- Invalid character
        END IF;
        
        -- Combine into bytes
        SET result = UNHEX(
            CONCAT(
                HEX(result),
                LPAD(HEX(val), 2, '0')
            )
        );
        
        SET i = i + 1;
    END WHILE;
    
    RETURN result;
END //
DELIMITER ;

-- Example usage:
-- SELECT HEX(ULID_TO_BIN('01FJYWZ3RJM927XKDJGDR06REA')); -- Convert string ULID to binary
-- SELECT BIN_TO_ULID(UNHEX('01860F9551D1A5C3E03D6FCB5D6C5119')); -- Convert binary to string ULID

-- Example table definition using BINARY(16) for ULIDs
CREATE TABLE IF NOT EXISTS users (
    id BINARY(16) PRIMARY KEY,
    ulid_str CHAR(26) GENERATED ALWAYS AS 
        (BIN_TO_ULID(id)) VIRTUAL,
    email VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    INDEX idx_id_binary (id),
    INDEX idx_ulid_str (ulid_str)
);

-- Example INSERT statement
INSERT INTO users (id, email) VALUES 
    (ULID_TO_BIN('01FJYWZ3RJM927XKDJGDR06REA'), 'user1@example.com');

-- Example SELECT with sorting by timestamp (automatically sorted when using binary format)
SELECT 
    BIN_TO_ULID(id) as ulid_string,
    HEX(id) as ulid_hex,
    email,
    created_at
FROM users
ORDER BY id; -- Natural chronological ordering

-- Example range query by timestamp
SELECT *
FROM users
WHERE id BETWEEN 
    ULID_TO_BIN('01FJYWZ0000000000000000000') -- Start of a time range
    AND 
    ULID_TO_BIN('01FJYWZZZZZZZZZZZZZZZZZZZZ'); -- End of a time range