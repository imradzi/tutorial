cmake_minimum_required(VERSION 3.21)
project(tut VERSION 0.9.0)

set(CMAKE_CXX_STANDARD 20)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Boost 1.88 COMPONENTS process system json filesystem serialization program_options REQUIRED)
find_package(fmt CONFIG REQUIRED)

message("boost libraries: ${Boost_LIBRARIES}")

add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} 
  -static-libgcc -static-libstdc++
  Threads::Threads
  fmt::fmt-header-only
  ${fmt_LIBRARIES}
  ${Boost_LIBRARIES}
)
