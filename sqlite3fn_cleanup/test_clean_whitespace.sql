-- Load the clean_whitespace extension
.load ./clean_whitespace

-- Enable column headers and better output formatting
.headers on
.mode column

-- Test 1: Multiple consecutive spaces
SELECT 'Test 1: Multiple spaces' AS test_case;
SELECT 'hello    world' AS input, clean_whitespace('hello    world') AS output;
SELECT '';

-- Test 2: Tabs and newlines
SELECT 'Test 2: Tabs and newlines' AS test_case;
SELECT 'hello	world
test' AS input, clean_whitespace('hello	world
test') AS output;
SELECT '';

-- Test 3: Leading and trailing whitespace
SELECT 'Test 3: Leading/trailing whitespace' AS test_case;
SELECT '  trim me  ' AS input, clean_whitespace('  trim me  ') AS output;
SELECT '';

-- Test 4: Mixed whitespace characters
SELECT 'Test 4: Mixed whitespace (CR, LF, tabs, spaces)' AS test_case;
SELECT 'text
	with		mixed   whitespace' AS input, clean_whitespace('text
	with		mixed   whitespace') AS output;
SELECT '';

-- Test 5: Reduce multiple spaces between words
SELECT 'Test 5: Multiple spaces between words' AS test_case;
SELECT 'reduce     multiple     spaces' AS input, clean_whitespace('reduce     multiple     spaces') AS output;
SELECT '';

-- Test 6: Empty string
SELECT 'Test 6: Empty string' AS test_case;
SELECT '' AS input, clean_whitespace('') AS output;
SELECT '';

-- Test 7: Only whitespace
SELECT 'Test 7: Only whitespace' AS test_case;
SELECT '     	
  ' AS input, clean_whitespace('     	
  ') AS output;
SELECT '';

-- Test 8: NULL handling
SELECT 'Test 8: NULL input' AS test_case;
SELECT NULL AS input, clean_whitespace(NULL) AS output;
SELECT '';

-- Test 9: No whitespace
SELECT 'Test 9: No whitespace to clean' AS test_case;
SELECT 'NoSpacesHere' AS input, clean_whitespace('NoSpacesHere') AS output;
SELECT '';

-- Test 10: Form feed and vertical tab
SELECT 'Test 10: Form feed and vertical tab' AS test_case;
SELECT 'beforeafter' AS input, clean_whitespace('beforeafter') AS output;
SELECT '';

-- Test 11: Real-world example with line breaks
SELECT 'Test 11: Multi-line text' AS test_case;
SELECT 'This is
  a   text
	with various    
whitespace issues.' AS input, clean_whitespace('This is
  a   text
	with various    
whitespace issues.') AS output;
SELECT '';

-- Test 12: Using with a table
SELECT 'Test 12: Using with table data' AS test_case;
CREATE TEMP TABLE test_data (id INTEGER, messy_text TEXT);
INSERT INTO test_data VALUES 
    (1, '  leading spaces'),
    (2, 'trailing spaces  '),
    (3, 'multiple    internal   spaces'),
    (4, 'tabs	and	spaces   mixed');

SELECT id, messy_text AS original, clean_whitespace(messy_text) AS cleaned FROM test_data;
