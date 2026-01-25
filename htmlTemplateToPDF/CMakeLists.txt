cmake_minimum_required(VERSION 3.16)
project(handlebars_pdf VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# wkhtmltopdf executable is required at runtime (not linked as library)
# Install via: sudo apt install wkhtmltopdf  (Linux)
#              choco install wkhtmltopdf     (Windows)
#              brew install wkhtmltopdf      (macOS)
find_program(WKHTMLTOPDF_EXECUTABLE wkhtmltopdf
    PATHS
        /usr/bin
        /usr/local/bin
        "C:/Program Files/wkhtmltopdf/bin"
        "$ENV{ProgramFiles}/wkhtmltopdf/bin"
)

if(WKHTMLTOPDF_EXECUTABLE)
    message(STATUS "Found wkhtmltopdf: ${WKHTMLTOPDF_EXECUTABLE}")
else()
    message(WARNING "wkhtmltopdf executable not found. PDF generation will fail at runtime unless wkhtmltopdf is in PATH.")
endif()

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/template_engine.cpp
    src/pdf_generator.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Copy templates directory to build folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/templates DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
