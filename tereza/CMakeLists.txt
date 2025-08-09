cmake_minimum_required(VERSION 3.21)
project(tut VERSION 0.9.0)

set(THREADS_PREFER_PTHREAD_FLAG ON)


add_executable(${PROJECT_NAME} "main.c")
target_link_libraries(${PROJECT_NAME})
