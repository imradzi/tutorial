cmake_minimum_required(VERSION 3.21)
project(tut VERSION 0.9.0)

set(CMAKE_CXX_STANDARD 20)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(CURL REQUIRED)
add_executable(${PROJECT_NAME} "main.cpp")

include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}  
  -static-libgcc -static-libstdc++
  Threads::Threads
  fmt::fmt-header-only
  ${fmt_LIBRARIES}
  ${CURL_LIBRARIES}
  -lcurl
  -ldl
  -lssl
  -lcrypto
)
