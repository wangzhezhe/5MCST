cmake_minimum_required(VERSION 3.14)
project(possion LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)

find_package(MPI REQUIRED) 

add_executable(possion2dhybrid 
main.cpp
possion2dhybrid.cpp
)
target_link_libraries(possion2dhybrid MPI::MPI_C)

