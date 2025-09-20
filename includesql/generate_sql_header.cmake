# generate_sql_header.cmake
# Arguments: CMAKE_ARGV3 = output header file, CMAKE_ARGV4 = input SQL file

# Read the SQL file content
file(READ "${CMAKE_ARGV4}" SQL_CONTENT)

# Escape the content properly for C++ string literal
# Replace backslashes first, then quotes, then newlines
string(REPLACE "\\" "\\\\" SQL_CONTENT_ESCAPED "${SQL_CONTENT}")
string(REPLACE "\"" "\\\"" SQL_CONTENT_ESCAPED "${SQL_CONTENT_ESCAPED}")
string(REPLACE "\n" "\\n" SQL_CONTENT_ESCAPED "${SQL_CONTENT_ESCAPED}")
string(REPLACE "\r" "\\r" SQL_CONTENT_ESCAPED "${SQL_CONTENT_ESCAPED}")
string(REPLACE "\t" "\\t" SQL_CONTENT_ESCAPED "${SQL_CONTENT_ESCAPED}")

# Write the header file with properly escaped content
file(WRITE "${CMAKE_ARGV3}" "#pragma once\n\nconst char* SQL_QUERY = \"${SQL_CONTENT_ESCAPED}\";\n")
