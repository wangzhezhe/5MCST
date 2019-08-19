cmake_minimum_required(VERSION 3.15)
project(mpitest LANGUAGES C CXX)

find_package(MPI REQUIRED) 

add_executable(testMPI testMPI.cxx)
# if the program is c, use MPI_C
target_link_libraries(testMPI MPI::MPI_CXX)



