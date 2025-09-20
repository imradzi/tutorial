cmake_minimum_required(VERSION 3.21)
project(tut VERSION 0.9.0)

set(SQL_FILE "${CMAKE_SOURCE_DIR}/my_query.sql")
set(GENERATED_HEADER "${CMAKE_BINARY_DIR}/generated_sql.h")

# Check if the SQL file exists
if(NOT EXISTS "${SQL_FILE}")
    message(FATAL_ERROR "Required file not found: ${SQL_FILE}")
endif()

# Add a custom command to generate the header file
add_custom_command(
    OUTPUT ${GENERATED_HEADER}
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/generate_sql_header.cmake ${GENERATED_HEADER} ${SQL_FILE}
    DEPENDS ${SQL_FILE}
    COMMENT "Generating SQL header from ${SQL_FILE}"
)

# Add a custom target to ensure the generated header is built
add_custom_target(generate_sql_header ALL DEPENDS ${GENERATED_HEADER})

# Set policy to avoid Boost warning
cmake_policy(SET CMP0167 NEW)

set(CMAKE_CXX_STANDARD 23)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Boost COMPONENTS system json filesystem serialization program_options REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(mysql-concpp REQUIRED)

add_executable(${PROJECT_NAME} "main.cpp" ${GENERATED_HEADER})
add_dependencies(${PROJECT_NAME} generate_sql_header)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    /usr/include/mysql-cppconn
)

target_link_libraries(${PROJECT_NAME} 
  -static-libgcc -static-libstdc++
  Threads::Threads
  fmt::fmt-header-only  nlohmann_json::nlohmann_json
  ${fmt_LIBRARIES}
  ${Boost_LIBRARIES}
  mysqlcppconnx
)
