cmake_minimum_required(VERSION 3.21)
project(tut VERSION 0.9.0)

set(CMAKE_CXX_STANDARD 23)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Boost COMPONENTS system json filesystem serialization program_options REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

find_path(MYSQLCPPCONN_INCLUDE_DIR mysqlx/xdevapi.h
          PATHS /usr/include/mysql-cppconn
                /usr/local/include/mysql-cppconn
                /opt/homebrew/include/mysql-cppconn)  # macOS Homebrew

find_library(MYSQLCPPCONN_LIBRARY NAMES mysqlcppconnx
             PATHS /usr/lib
                   /usr/lib/x86_64-linux-gnu
                   /usr/local/lib
                   /opt/homebrew/lib)  # macOS Homebrew

if(MYSQLCPPCONN_INCLUDE_DIR AND MYSQLCPPCONN_LIBRARY)
    message(STATUS "Found MySQL C++ Connector: Lib: ${MYSQLCPPCONN_LIBRARY} ; path: ${MYSQLCPPCONN_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "MySQL C++ Connector not found!")
endif()

add_executable(${PROJECT_NAME} "main.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MYSQLCPPCONN_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} 
  -static-libgcc -static-libstdc++
  Threads::Threads
  fmt::fmt-header-only  nlohmann_json::nlohmann_json
  ${fmt_LIBRARIES}
  ${Boost_LIBRARIES}
  ${MYSQLCPPCONN_LIBRARY}    

)
