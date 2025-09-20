# SqlGeneration.cmake - CMake functions for generating C++ files from SQL files
# 
# This module provides functions to convert SQL files into C++ source and header files
# with proper escaping and dependency handling for efficient incremental builds.

# Store the path to the script in this module's directory
set(_SQL_GENERATION_SCRIPT_PATH "${CMAKE_CURRENT_LIST_DIR}/generate_sql_header.cmake")

#[[
Function: add_sql_sources
Generate C++ source and header files from SQL files.

Usage:
    add_sql_sources(
        OUTPUT_SOURCES <variable_name>
        OUTPUT_HEADERS <variable_name>
        SQL_FILES <file1.sql> <file2.sql> ...
        CPP_FILES <file1.cpp> <file2.cpp> ...
        VARIABLE_NAMES <VAR1> <VAR2> ...
    )

Parameters:
    OUTPUT_SOURCES  - Variable to store the list of generated .cpp files
    OUTPUT_HEADERS  - Variable to store the list of generated .h files
    SQL_FILES      - List of input SQL files (relative to CMAKE_SOURCE_DIR)
    CPP_FILES      - List of output .cpp filenames (will be in CMAKE_BINARY_DIR)
    VARIABLE_NAMES - List of C++ variable names to use in headers

Example:
    add_sql_sources(
        OUTPUT_SOURCES GENERATED_SOURCES
        OUTPUT_HEADERS GENERATED_HEADERS  
        SQL_FILES "query1.sql" "query2.sql"
        CPP_FILES "bkk_query1.cpp" "bkk_query2.cpp"
        VARIABLE_NAMES "bkk_QUERY1" "bkk_QUERY2"
    )
#]]
function(add_sql_sources)
    # Parse arguments
    cmake_parse_arguments(ARGS "" "OUTPUT_SOURCES;OUTPUT_HEADERS" "SQL_FILES;CPP_FILES;VARIABLE_NAMES" ${ARGN})
    
    # Validate arguments
    if(NOT ARGS_OUTPUT_SOURCES OR NOT ARGS_OUTPUT_HEADERS)
        message(FATAL_ERROR "add_sql_sources: OUTPUT_SOURCES and OUTPUT_HEADERS are required")
    endif()
    
    if(NOT ARGS_SQL_FILES OR NOT ARGS_CPP_FILES OR NOT ARGS_VARIABLE_NAMES)
        message(FATAL_ERROR "add_sql_sources: SQL_FILES, CPP_FILES, and VARIABLE_NAMES are required")
    endif()
    
    # Check that all lists have the same length
    list(LENGTH ARGS_SQL_FILES sql_count)
    list(LENGTH ARGS_CPP_FILES cpp_count)
    list(LENGTH ARGS_VARIABLE_NAMES var_count)
    
    if(NOT sql_count EQUAL cpp_count OR NOT sql_count EQUAL var_count)
        message(FATAL_ERROR "add_sql_sources: SQL_FILES, CPP_FILES, and VARIABLE_NAMES must have the same number of elements")
    endif()
    
    # Verify all SQL files exist
    foreach(sql_file IN LISTS ARGS_SQL_FILES)
        if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${sql_file}")
            message(FATAL_ERROR "Required SQL file not found: ${CMAKE_SOURCE_DIR}/${sql_file}")
        endif()
    endforeach()
    
    # Generate the files
    set(generated_sources "")
    set(generated_headers "")
    math(EXPR max_index "${sql_count} - 1")
    
    foreach(index RANGE ${max_index})
        list(GET ARGS_SQL_FILES ${index} sql_file)
        list(GET ARGS_CPP_FILES ${index} cpp_file)
        list(GET ARGS_VARIABLE_NAMES ${index} variable_name)
        
        set(sql_path "${CMAKE_SOURCE_DIR}/${sql_file}")
        set(cpp_path "${CMAKE_BINARY_DIR}/${cpp_file}")
        
        # Derive header file path from cpp file path
        get_filename_component(header_name "${cpp_file}" NAME_WE)
        set(header_path "${CMAKE_BINARY_DIR}/${header_name}.h")
        
        # Create custom command for this SQL->cpp+header conversion
        add_custom_command(
            OUTPUT ${cpp_path} ${header_path}
            COMMAND ${CMAKE_COMMAND} -P ${_SQL_GENERATION_SCRIPT_PATH} ${cpp_path} ${sql_path} ${variable_name}
            DEPENDS ${sql_path}
            COMMENT "Generating ${cpp_file} and ${header_name}.h from ${sql_file}"
        )
        
        list(APPEND generated_sources ${cpp_path})
        list(APPEND generated_headers ${header_path})
    endforeach()
    
    # Return the generated file lists to the parent scope
    set(${ARGS_OUTPUT_SOURCES} ${generated_sources} PARENT_SCOPE)
    set(${ARGS_OUTPUT_HEADERS} ${generated_headers} PARENT_SCOPE)
endfunction()

#[[
Function: add_sql_target
Create a custom target that depends on all generated SQL files.

Usage:
    add_sql_target(
        TARGET_NAME <target_name>
        SOURCES <source_list>
        HEADERS <header_list>
    )

Parameters:
    TARGET_NAME - Name of the custom target to create
    SOURCES     - List of generated source files
    HEADERS     - List of generated header files

Example:
    add_sql_target(
        TARGET_NAME generate_sql_sources
        SOURCES ${GENERATED_SOURCES}
        HEADERS ${GENERATED_HEADERS}
    )
#]]
function(add_sql_target)
    cmake_parse_arguments(ARGS "" "TARGET_NAME" "SOURCES;HEADERS" ${ARGN})
    
    if(NOT ARGS_TARGET_NAME)
        message(FATAL_ERROR "add_sql_target: TARGET_NAME is required")
    endif()
    
    add_custom_target(${ARGS_TARGET_NAME} ALL 
        DEPENDS ${ARGS_SOURCES} ${ARGS_HEADERS}
        COMMENT "Generating all SQL source files"
    )
endfunction()

#[[
Convenience macro: setup_sql_generation
Complete setup for SQL generation - combines add_sql_sources and add_sql_target.

Usage:
    setup_sql_generation(
        TARGET_NAME <target_name>
        OUTPUT_SOURCES <variable_name>
        SQL_FILES <file1.sql> <file2.sql> ...
        CPP_FILES <file1.cpp> <file2.cpp> ...
        VARIABLE_NAMES <VAR1> <VAR2> ...
    )

This macro will:
1. Generate the source and header files
2. Create a custom target
3. Set the OUTPUT_SOURCES variable in parent scope

Example:
    setup_sql_generation(
        TARGET_NAME generate_sql_sources
        OUTPUT_SOURCES GENERATED_SOURCES
        SQL_FILES "query1.sql" "query2.sql"
        CPP_FILES "bkk_query1.cpp" "bkk_query2.cpp"
        VARIABLE_NAMES "bkk_QUERY1" "bkk_QUERY2"
    )
#]]
macro(setup_sql_generation)
    cmake_parse_arguments(ARGS "" "TARGET_NAME;OUTPUT_SOURCES" "SQL_FILES;CPP_FILES;VARIABLE_NAMES" ${ARGN})
    
    if(NOT ARGS_TARGET_NAME OR NOT ARGS_OUTPUT_SOURCES)
        message(FATAL_ERROR "setup_sql_generation: TARGET_NAME and OUTPUT_SOURCES are required")
    endif()
    
    # Generate the files
    add_sql_sources(
        OUTPUT_SOURCES _generated_sources
        OUTPUT_HEADERS _generated_headers
        SQL_FILES ${ARGS_SQL_FILES}
        CPP_FILES ${ARGS_CPP_FILES}
        VARIABLE_NAMES ${ARGS_VARIABLE_NAMES}
    )
    
    # Create the target
    add_sql_target(
        TARGET_NAME ${ARGS_TARGET_NAME}
        SOURCES ${_generated_sources}
        HEADERS ${_generated_headers}
    )
    
    # Return sources to parent scope
    set(${ARGS_OUTPUT_SOURCES} ${_generated_sources})
endmacro()