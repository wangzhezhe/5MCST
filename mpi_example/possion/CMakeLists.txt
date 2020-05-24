cmake_minimum_required(VERSION 3.14)
project(possion LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)

find_package(MPI REQUIRED) 

add_executable(possion2d 
main.cpp
possion2d.cpp
)
target_link_libraries(possion2d MPI::MPI_C)

