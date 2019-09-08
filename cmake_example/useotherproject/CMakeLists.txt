cmake_minimum_required (VERSION 3.2)
project (tutorial LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

find_package(RapidJSON REQUIRED)

message ("info")
message (${RapidJSON_INCLUDE_DIRS})

add_executable(main main.cpp)
target_link_libraries(main)
target_include_directories(main PUBLIC $<BUILD_INTERFACE:${RapidJSON_INCLUDE_DIRS}>)



