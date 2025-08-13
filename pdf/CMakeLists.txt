cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0167 NEW)
project(tut VERSION 0.9.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_path(HPDF_INCLUDE_DIR hpdf.h PATHS /usr/local/include /opt/libharu/include NO_DEFAULT_PATH)
find_library(HPDF_LIBRARY NAMES hpdf libhpdf PATHS /usr/local/lib /opt/libharu/lib NO_DEFAULT_PATH)

if (NOT HPDF_INCLUDE_DIR OR NOT HPDF_LIBRARY)
    message(FATAL_ERROR "libHaru not found. Please set HPDF_INCLUDE_DIR and HPDF_LIBRARY.")
endif()

find_package(Boost REQUIRED COMPONENTS system json filesystem serialization program_options)
find_package(fmt CONFIG REQUIRED)

# Find libpng
find_package(PNG REQUIRED)


add_executable(${PROJECT_NAME} 
"src/PDFWriter.cpp"
"src/PDFPage.cpp"
"src/PDFFont.cpp"
"src/PDFError.cpp"
"src/PDFCell.cpp"
"qrcodegen.cpp"
"src/qrgen.cpp"
"main.cpp"
)

target_include_directories(${PROJECT_NAME} PUBLIC ${HPDF_INCLUDE_DIR} "${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}")
target_link_libraries(${PROJECT_NAME} 
  -static-libgcc -static-libstdc++
  Threads::Threads
  fmt::fmt-header-only
  ${fmt_LIBRARIES}
  ${Boost_LIBRARIES}
  ${HPDF_LIBRARY}
  PNG::PNG
)
