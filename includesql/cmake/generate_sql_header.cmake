# generate_sql_header.cmake
# Arguments: 
#   CMAKE_ARGV3 = output cpp file path
#   CMAKE_ARGV4 = input SQL file path  
#   CMAKE_ARGV5 = variable name to use 

# Set default variable name if not provided
if(NOT CMAKE_ARGV5)
    set(VARIABLE_NAME "SQL_QUERY")
else()
    set(VARIABLE_NAME "${CMAKE_ARGV5}")
endif()

# Extract paths for .cpp and .h files
get_filename_component(OUTPUT_DIR "${CMAKE_ARGV3}" DIRECTORY)
get_filename_component(OUTPUT_NAME "${CMAKE_ARGV3}" NAME_WE)
set(CPP_FILE "${CMAKE_ARGV3}")
set(HEADER_FILE "${OUTPUT_DIR}/${OUTPUT_NAME}.h")

# Read the SQL file content
file(READ "${CMAKE_ARGV4}" SQL_CONTENT)

# Escape the content properly for C++ string literal
# Replace backslashes first, then quotes, then newlines
string(REPLACE "\\" "\\\\" SQL_CONTENT_ESCAPED "${SQL_CONTENT}")
string(REPLACE "\"" "\\\"" SQL_CONTENT_ESCAPED "${SQL_CONTENT_ESCAPED}")
string(REPLACE "\n" "\\n" SQL_CONTENT_ESCAPED "${SQL_CONTENT_ESCAPED}")
string(REPLACE "\r" "\\r" SQL_CONTENT_ESCAPED "${SQL_CONTENT_ESCAPED}")
string(REPLACE "\t" "\\t" SQL_CONTENT_ESCAPED "${SQL_CONTENT_ESCAPED}")

# Write the header file (small, just declaration)
file(WRITE "${HEADER_FILE}" "#pragma once\n\n// Generated from ${CMAKE_ARGV4}\nextern const char* ${VARIABLE_NAME};\n")

# Write the cpp file (contains the actual SQL data)
file(WRITE "${CPP_FILE}" "// Generated from ${CMAKE_ARGV4}\n#include \"${OUTPUT_NAME}.h\"\n\nconst char* ${VARIABLE_NAME} = \"${SQL_CONTENT_ESCAPED}\";\n")
