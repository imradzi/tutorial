cmake_minimum_required(VERSION 3.16)
project(handlebars_pdf VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find libwkhtmltox
# Install from: https://github.com/wkhtmltopdf/packaging/releases
# Ubuntu: sudo dpkg -i wkhtmltox_*.deb && sudo apt -f install
find_path(WKHTMLTOX_INCLUDE_DIR wkhtmltox/pdf.h
    PATHS /usr/include /usr/local/include
)
find_library(WKHTMLTOX_LIBRARY NAMES wkhtmltox
    PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
)

if(NOT WKHTMLTOX_INCLUDE_DIR OR NOT WKHTMLTOX_LIBRARY)
    message(FATAL_ERROR "libwkhtmltox not found. Install from https://github.com/wkhtmltopdf/packaging/releases")
endif()

message(STATUS "Found libwkhtmltox: ${WKHTMLTOX_LIBRARY}")

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/template_engine.cpp
    src/pdf_generator.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${WKHTMLTOX_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${WKHTMLTOX_LIBRARY}
)

# Copy templates directory to build folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/templates DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
