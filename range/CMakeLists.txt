cmake_minimum_required(VERSION 3.21)
project(tut VERSION 0.9.0)

set(CMAKE_CXX_STANDARD 20)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Boost 1.80 COMPONENTS system filesystem serialization program_options REQUIRED)
find_package(fmt CONFIG REQUIRED)


add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} 
  Threads::Threads
  fmt::fmt-header-only ${Boost_LIBRARIES}
)
