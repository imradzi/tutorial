cmake_minimum_required(VERSION 3.17)
project(tut)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Boost 1.80 COMPONENTS system filesystem program_options serialization coroutine regex REQUIRED)
find_package(fmt CONFIG REQUIRED)


add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} Threads::Threads ${Boost_LIBRARIES} fmt::fmt-header-only) 
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
