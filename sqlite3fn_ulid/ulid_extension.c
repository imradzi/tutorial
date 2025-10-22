#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1

#include <stdint.h>
#include <string.h>
#include <ctype.h>

// Crockford Base32 alphabet (no I, L, O, U)
static const char B32_ALPHABET[] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";

// Decode table for Base32 - 0xFF means invalid
static const uint8_t B32_DECODE[256] = {
    // 0x00-0x0F
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    // 0x10-0x1F  
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    // 0x20-0x2F (space and symbols)
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    // 0x30-0x3F ('0'-'9' and symbols)
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    // 0x40-0x4F ('@','A'-'F')
    0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x01, 0x12, 0x13, 0x01, 0x14, 0x15, 0x00,
    // 0x50-0x5F ('P'-'Z' and symbols)  
    0x16, 0x17, 0x18, 0x19, 0x1A, 0xFF, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    // 0x60-0x6F ('`','a'-'f')
    0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x01, 0x12, 0x13, 0x01, 0x14, 0x15, 0x00,
    // 0x70-0x7F ('p'-'z' and DEL)
    0x16, 0x17, 0x18, 0x19, 0x1A, 0xFF, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    // 0x80-0xFF (high ASCII - all invalid)
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

// Helper: Read 48-bit timestamp from first 6 bytes of ULID (big-endian)
static uint64_t read_ts48_be(const uint8_t ulid[16]) {
    return ((uint64_t)ulid[0] << 40) |
           ((uint64_t)ulid[1] << 32) |
           ((uint64_t)ulid[2] << 24) |
           ((uint64_t)ulid[3] << 16) |
           ((uint64_t)ulid[4] << 8) |
           ((uint64_t)ulid[5]);
}

// Helper: Sanitize input string (remove hyphens/spaces, convert to uppercase)
static int sanitize_ulid_string(const char* input, char* output, int max_len) {
    int j = 0;
    for (int i = 0; input[i] != '\0' && j < max_len - 1; i++) {
        char c = input[i];
        if (c == '-' || c == ' ') {
            continue; // Skip hyphens and spaces
        }
        output[j++] = toupper(c);
    }
    output[j] = '\0';
    return j;
}

// SQLite function: ulid_to_string(blob16) -> text26
static void ulid_to_string_sql(sqlite3_context* context, int argc, sqlite3_value** argv) {
    if (argc != 1) {
        sqlite3_result_error(context, "ulid_to_string: expected 1 argument", -1);
        return;
    }
    
    if (sqlite3_value_type(argv[0]) == SQLITE_NULL) {
        sqlite3_result_null(context);
        return;
    }
    
    if (sqlite3_value_type(argv[0]) != SQLITE_BLOB) {
        sqlite3_result_error(context, "ulid_to_string: argument must be a BLOB", -1);
        return;
    }
    
    const uint8_t* blob = sqlite3_value_blob(argv[0]);
    int blob_len = sqlite3_value_bytes(argv[0]);
    
    if (blob_len != 16) {
        sqlite3_result_error(context, "ulid_to_string: BLOB must be exactly 16 bytes", -1);
        return;
    }
    
    // Bit-streaming encoding: treat entire 16 bytes as continuous 128-bit stream
    char result[27]; // 26 chars + null terminator
    uint64_t accumulator = 0;
    int bits_accumulated = 0;
    int result_idx = 0;
    
    for (int i = 0; i < 16; i++) {
        accumulator = (accumulator << 8) | blob[i];
        bits_accumulated += 8;
        
        while (bits_accumulated >= 5) {
            bits_accumulated -= 5;
            result[result_idx++] = B32_ALPHABET[(accumulator >> bits_accumulated) & 0x1F];
        }
    }
    
    // Handle remaining bits (if any) by left-padding with zeros
    if (bits_accumulated > 0) {
        result[result_idx++] = B32_ALPHABET[(accumulator << (5 - bits_accumulated)) & 0x1F];
    }
    
    // Pad to 26 characters if needed
    while (result_idx < 26) {
        result[result_idx++] = '0';
    }
    
    result[26] = '\0';
    sqlite3_result_text(context, result, 26, SQLITE_TRANSIENT);
}

// SQLite function: ulid_from_string(text26) -> blob16
static void ulid_from_string_sql(sqlite3_context* context, int argc, sqlite3_value** argv) {
    if (argc != 1) {
        sqlite3_result_error(context, "ulid_from_string: expected 1 argument", -1);
        return;
    }
    
    if (sqlite3_value_type(argv[0]) == SQLITE_NULL) {
        sqlite3_result_null(context);
        return;
    }
    
    if (sqlite3_value_type(argv[0]) != SQLITE_TEXT) {
        sqlite3_result_error(context, "ulid_from_string: argument must be TEXT", -1);
        return;
    }
    
    const char* input = (const char*)sqlite3_value_text(argv[0]);
    char sanitized[128];
    int len = sanitize_ulid_string(input, sanitized, sizeof(sanitized));
    
    if (len != 26) {
        sqlite3_result_error(context, "ulid_from_string: input must be exactly 26 characters after sanitization", -1);
        return;
    }
    
    // Bit-streaming decoding: treat 26 Base32 chars as continuous bit stream
    uint8_t result[16];
    uint64_t accumulator = 0;
    int bits_accumulated = 0;
    int byte_index = 0;
    
    for (int i = 0; i < 26; i++) {
        uint8_t val = B32_DECODE[(unsigned char)sanitized[i]];
        if (val == 0xFF) {
            sqlite3_result_error(context, "ulid_from_string: invalid character in ULID", -1);
            return;
        }
        
        // Add 5 bits to accumulator
        accumulator = (accumulator << 5) | val;
        bits_accumulated += 5;
        
        // Extract complete bytes (8 bits)
        while (bits_accumulated >= 8 && byte_index < 16) {
            bits_accumulated -= 8;
            result[byte_index++] = (uint8_t)((accumulator >> bits_accumulated) & 0xFF);
        }
    }
    
    sqlite3_result_blob(context, result, 16, SQLITE_TRANSIENT);
}

// SQLite function: ulid_timestamp(blob16) -> integer
static void ulid_timestamp_sql(sqlite3_context* context, int argc, sqlite3_value** argv) {
    if (argc != 1) {
        sqlite3_result_error(context, "ulid_timestamp: expected 1 argument", -1);
        return;
    }
    
    if (sqlite3_value_type(argv[0]) == SQLITE_NULL) {
        sqlite3_result_null(context);
        return;
    }
    
    if (sqlite3_value_type(argv[0]) != SQLITE_BLOB) {
        sqlite3_result_error(context, "ulid_timestamp: argument must be a BLOB", -1);
        return;
    }
    
    const uint8_t* blob = sqlite3_value_blob(argv[0]);
    int blob_len = sqlite3_value_bytes(argv[0]);
    
    if (blob_len != 16) {
        sqlite3_result_error(context, "ulid_timestamp: BLOB must be exactly 16 bytes", -1);
        return;
    }
    
    uint64_t ts = read_ts48_be(blob);
    sqlite3_result_int64(context, (sqlite3_int64)ts);
}

// SQLite function: ulid_randomness(blob16) -> blob10
static void ulid_randomness_sql(sqlite3_context* context, int argc, sqlite3_value** argv) {
    if (argc != 1) {
        sqlite3_result_error(context, "ulid_randomness: expected 1 argument", -1);
        return;
    }
    
    if (sqlite3_value_type(argv[0]) == SQLITE_NULL) {
        sqlite3_result_null(context);
        return;
    }
    
    if (sqlite3_value_type(argv[0]) != SQLITE_BLOB) {
        sqlite3_result_error(context, "ulid_randomness: argument must be a BLOB", -1);
        return;
    }
    
    const uint8_t* blob = sqlite3_value_blob(argv[0]);
    int blob_len = sqlite3_value_bytes(argv[0]);
    
    if (blob_len != 16) {
        sqlite3_result_error(context, "ulid_randomness: BLOB must be exactly 16 bytes", -1);
        return;
    }
    
    // Return last 10 bytes as BLOB
    sqlite3_result_blob(context, blob + 6, 10, SQLITE_TRANSIENT);
}

// Extension entry point  
__attribute__((visibility("default")))
int sqlite3_ulidextension_init(sqlite3* db, char** pzErrMsg, const sqlite3_api_routines* pApi) {
    (void)pzErrMsg; // Suppress unused parameter warning
    SQLITE_EXTENSION_INIT2(pApi);
    
    int rc = SQLITE_OK;
    
    rc |= sqlite3_create_function_v2(db, "ulid_to_string", 1, 
                                   SQLITE_UTF8 | SQLITE_DETERMINISTIC, 
                                   NULL, ulid_to_string_sql, NULL, NULL, NULL);
    
    rc |= sqlite3_create_function_v2(db, "ulid_from_string", 1, 
                                   SQLITE_UTF8 | SQLITE_DETERMINISTIC, 
                                   NULL, ulid_from_string_sql, NULL, NULL, NULL);
    
    rc |= sqlite3_create_function_v2(db, "ulid_timestamp", 1, 
                                   SQLITE_UTF8 | SQLITE_DETERMINISTIC, 
                                   NULL, ulid_timestamp_sql, NULL, NULL, NULL);
    
    rc |= sqlite3_create_function_v2(db, "ulid_randomness", 1, 
                                   SQLITE_UTF8 | SQLITE_DETERMINISTIC, 
                                   NULL, ulid_randomness_sql, NULL, NULL, NULL);
    
    return rc ? SQLITE_ERROR : SQLITE_OK;
}