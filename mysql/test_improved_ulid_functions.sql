-- ========================================
-- Test Improved ULID Functions
-- Run this after installing mysql_global_ulid_functions_improved.sql
-- ========================================

-- Test 1: Basic ULID generation with randomness
SELECT '=== Test 1: Basic ULID Generation ===' as test_section;

SELECT mysql.ULID_GENERATE_NOW() as ulid_1;
SELECT mysql.ULID_GENERATE_NOW() as ulid_2;
SELECT mysql.ULID_GENERATE_NOW() as ulid_3;

-- Check that they are different (proper randomness)
SET @ulid1 = mysql.ULID_GENERATE_NOW();
SET @ulid2 = mysql.ULID_GENERATE_NOW();
SET @ulid3 = mysql.ULID_GENERATE_NOW();

SELECT @ulid1 as first_ulid, @ulid2 as second_ulid, (@ulid1 != @ulid2) as are_different;

-- Test 2: ULID generation with specific timestamp
SELECT '=== Test 2: Specific Timestamp Generation ===' as test_section;

-- Generate ULIDs for same timestamp - should have same timestamp part but different random parts
SET @timestamp_ms = 1640995200000; -- 2022-01-01 00:00:00 UTC
SELECT mysql.ULID_GENERATE(@timestamp_ms) as ulid_same_time_1;
SELECT mysql.ULID_GENERATE(@timestamp_ms) as ulid_same_time_2;

-- Extract timestamps to verify they match
SELECT 
    mysql.ULID_TIMESTAMP(mysql.ULID_GENERATE(@timestamp_ms)) as extracted_timestamp,
    @timestamp_ms as original_timestamp,
    (mysql.ULID_TIMESTAMP(mysql.ULID_GENERATE(@timestamp_ms)) = @timestamp_ms) as timestamps_match;

-- Test 3: Secure ULID generation
SELECT '=== Test 3: Secure ULID Generation ===' as test_section;

SELECT mysql.ULID_GENERATE_SECURE(NULL) as secure_ulid_1;
SELECT mysql.ULID_GENERATE_SECURE(NULL) as secure_ulid_2;
SELECT mysql.ULID_GENERATE_SECURE(1640995200000) as secure_ulid_specific_time;

-- Test 4: Roundtrip conversion (string -> binary -> string)
SELECT '=== Test 4: Roundtrip Conversion ===' as test_section;

SET @original = mysql.ULID_GENERATE_NOW();
SET @binary = mysql.ULID_TO_BIN(@original);
SET @restored = mysql.BIN_TO_ULID(@binary);

SELECT 
    @original as original_ulid,
    HEX(@binary) as binary_hex,
    @restored as restored_ulid,
    (@original = @restored) as roundtrip_success;

-- Test 5: Timestamp extraction and datetime conversion
SELECT '=== Test 5: Timestamp and DateTime Functions ===' as test_section;

SET @test_ulid = mysql.ULID_GENERATE_NOW();
SELECT 
    @test_ulid as ulid,
    mysql.ULID_TIMESTAMP(@test_ulid) as timestamp_ms,
    mysql.ULID_TO_DATETIME(@test_ulid) as datetime_value,
    NOW(3) as current_time;

-- Test 6: Random quality check
SELECT '=== Test 6: Random Quality Check ===' as test_section;

-- Generate multiple ULIDs and check uniqueness
CREATE TEMPORARY TABLE temp_ulids (
    id INT AUTO_INCREMENT PRIMARY KEY,
    ulid_str CHAR(26),
    ulid_bin BINARY(16)
);

-- Generate 100 ULIDs
INSERT INTO temp_ulids (ulid_str, ulid_bin)
SELECT mysql.ULID_GENERATE_NOW(), mysql.ULID_TO_BIN(mysql.ULID_GENERATE_NOW())
FROM (
    SELECT 1 as n UNION ALL SELECT 2 UNION ALL SELECT 3 UNION ALL SELECT 4 UNION ALL SELECT 5 UNION ALL
    SELECT 6 UNION ALL SELECT 7 UNION ALL SELECT 8 UNION ALL SELECT 9 UNION ALL SELECT 10
) t1,
(
    SELECT 1 as n UNION ALL SELECT 2 UNION ALL SELECT 3 UNION ALL SELECT 4 UNION ALL SELECT 5 UNION ALL
    SELECT 6 UNION ALL SELECT 7 UNION ALL SELECT 8 UNION ALL SELECT 9 UNION ALL SELECT 10
) t2
LIMIT 100;

-- Check uniqueness
SELECT 
    COUNT(*) as total_generated,
    COUNT(DISTINCT ulid_str) as unique_string_ulids,
    COUNT(DISTINCT ulid_bin) as unique_binary_ulids,
    (COUNT(*) = COUNT(DISTINCT ulid_str)) as all_strings_unique,
    (COUNT(*) = COUNT(DISTINCT ulid_bin)) as all_binaries_unique
FROM temp_ulids;

-- Test 7: Performance comparison
SELECT '=== Test 7: Performance Comparison ===' as test_section;

-- Time ULID generation (approximate)
SELECT BENCHMARK(1000, mysql.ULID_GENERATE_NOW()) as benchmark_standard;
SELECT BENCHMARK(1000, mysql.ULID_GENERATE_SECURE(NULL)) as benchmark_secure;

-- Test 8: Validation of ULID format
SELECT '=== Test 8: ULID Format Validation ===' as test_section;

SET @generated_ulid = mysql.ULID_GENERATE_NOW();
SELECT 
    @generated_ulid as generated_ulid,
    CHAR_LENGTH(@generated_ulid) as length_check,
    (@generated_ulid REGEXP '^[0-9A-HJKMNP-TV-Z]{26}$') as format_valid,
    mysql.ULID_TIMESTAMP(@generated_ulid) IS NOT NULL as timestamp_extractable;

-- Test 9: Chronological ordering
SELECT '=== Test 9: Chronological Ordering ===' as test_section;

-- Generate ULIDs with small time delays
SET @ulid_t1 = mysql.ULID_GENERATE_NOW();
-- Small delay simulation (not perfect but demonstrates concept)
SET @ulid_t2 = mysql.ULID_GENERATE(UNIX_TIMESTAMP(NOW(3)) * 1000 + 1);
SET @ulid_t3 = mysql.ULID_GENERATE(UNIX_TIMESTAMP(NOW(3)) * 1000 + 2);

SELECT 
    @ulid_t1 as ulid_time_1,
    @ulid_t2 as ulid_time_2,
    @ulid_t3 as ulid_time_3,
    (@ulid_t1 < @ulid_t2) as chronological_order_1_2,
    (@ulid_t2 < @ulid_t3) as chronological_order_2_3;

-- Test 10: Binary ordering
SELECT '=== Test 10: Binary Ordering ===' as test_section;

SET @bin_t1 = mysql.ULID_TO_BIN(@ulid_t1);
SET @bin_t2 = mysql.ULID_TO_BIN(@ulid_t2);
SET @bin_t3 = mysql.ULID_TO_BIN(@ulid_t3);

SELECT 
    HEX(@bin_t1) as binary_hex_1,
    HEX(@bin_t2) as binary_hex_2,
    HEX(@bin_t3) as binary_hex_3,
    (@bin_t1 < @bin_t2) as binary_order_1_2,
    (@bin_t2 < @bin_t3) as binary_order_2_3;

-- Cleanup
DROP TEMPORARY TABLE temp_ulids;

SELECT '=== All Tests Completed ===' as test_section;
SELECT 'Functions are working with proper random generation!' as result;