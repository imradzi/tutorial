#include "../sqlite3fn_ulid/sqlite3ext.h"
SQLITE_EXTENSION_INIT1

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Helper: Check if character is any whitespace (space, tab, CR, LF, form feed, vertical tab)
static int is_whitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' || c == '\v');
}

// SQLite function: clean_whitespace(text) -> text
// Replaces all whitespace characters with single spaces, reduces multiple spaces to one,
// and trims leading/trailing whitespace
static void clean_whitespace_sql(sqlite3_context* context, int argc, sqlite3_value** argv) {
    if (argc != 1) {
        sqlite3_result_error(context, "clean_whitespace: expected 1 argument", -1);
        return;
    }
    
    if (sqlite3_value_type(argv[0]) == SQLITE_NULL) {
        sqlite3_result_null(context);
        return;
    }
    
    if (sqlite3_value_type(argv[0]) != SQLITE_TEXT) {
        sqlite3_result_error(context, "clean_whitespace: argument must be TEXT", -1);
        return;
    }
    
    const char* input = (const char*)sqlite3_value_text(argv[0]);
    int input_len = sqlite3_value_bytes(argv[0]);
    
    // Allocate buffer for result (worst case: same size as input)
    char* result = (char*)sqlite3_malloc(input_len + 1);
    if (!result) {
        sqlite3_result_error_nomem(context);
        return;
    }
    
    int j = 0;  // Index for result buffer
    int last_was_space = 1;  // Start as true to trim leading whitespace
    
    // Process each character
    for (int i = 0; i < input_len; i++) {
        char c = input[i];
        
        if (is_whitespace(c)) {
            // Convert any whitespace to space, but only add if previous wasn't space
            if (!last_was_space) {
                result[j++] = ' ';
                last_was_space = 1;
            }
        } else {
            // Non-whitespace character: add it directly
            result[j++] = c;
            last_was_space = 0;
        }
    }
    
    // Trim trailing whitespace (remove last space if present)
    if (j > 0 && result[j - 1] == ' ') {
        j--;
    }
    
    result[j] = '\0';
    
    // Return the result
    sqlite3_result_text(context, result, j, sqlite3_free);
}

// Extension entry point
__attribute__((visibility("default")))
int sqlite3_cleanwhitespace_init(sqlite3* db, char** pzErrMsg, const sqlite3_api_routines* pApi) {
    (void)pzErrMsg;  // Suppress unused parameter warning
    SQLITE_EXTENSION_INIT2(pApi);
    
    int rc = sqlite3_create_function_v2(db, "clean_whitespace", 1,
                                       SQLITE_UTF8 | SQLITE_DETERMINISTIC,
                                       NULL, clean_whitespace_sql, NULL, NULL, NULL);
    
    return rc;
}
