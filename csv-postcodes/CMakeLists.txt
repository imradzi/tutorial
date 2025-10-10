cmake_minimum_required(VERSION 3.21)
project(tut VERSION 0.9.0)

set(CMAKE_CXX_STANDARD 23)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)


add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} 
  -static-libgcc -static-libstdc++
  Threads::Threads
  fmt::fmt-header-only  nlohmann_json::nlohmann_json
  ${fmt_LIBRARIES}
  ${Boost_LIBRARIES}
)

# Add CSV reader executable
add_executable(csv_reader "csv_reader.cpp")
# CSV reader doesn't need external libraries, just standard C++

# Add simple CSV reader executable (corrected version)
add_executable(csv_reader_simple "csv_reader_simple.cpp")

# Add CSV reader with arrays version
add_executable(csv_reader_arrays "csv_reader_arrays.cpp")
