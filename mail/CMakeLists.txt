cmake_minimum_required(VERSION 3.21)
project(AuthenticationManager VERSION 0.9.0)

message("CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}")

set(CMAKE_CXX_STANDARD 20)

find_package(mailio CONFIG REQUIRED)
find_package(Boost COMPONENTS system filesystem program_options serialization date_time regex REQUIRED)
find_package(fmt CONFIG REQUIRED)
message("cmake lib path = ${CMAKE_LIBRARY_PATH}")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fno-common -pedantic -pedantic-errors -Wall -Wextra -Werror \
  -Wno-deprecated-enum-enum-conversion -Wno-missing-field-initializers -Wmissing-include-dirs -Woverloaded-virtual -Wredundant-decls \
	-Wshadow -Wswitch-default -Wundef -Wno-switch-default -Wno-unused-parameter -Wno-shadow -Wno-unused-but-set-parameter")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(mail main.cpp)
target_link_libraries(mail fmt::fmt-header-only ${Boost_LIBRARIES} mailio
        -luuid
        -ldl
        -lssl
        -lcrypto
    )

target_include_directories(mail PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(mail PUBLIC -DPARTIO_USE_ZLIB -DNO_REPORT -DNO_XLS -DSQLITE_ENABLE_FTS5 -DSQLITE_ENABLE_FTS4)
