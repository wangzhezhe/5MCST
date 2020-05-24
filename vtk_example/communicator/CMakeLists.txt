cmake_minimum_required (VERSION 3.14)
project (vtkcommunicator LANGUAGES C CXX)

find_package(MPI REQUIRED)

find_package(VTK COMPONENTS
CommonDataModel
ParallelMPI
FiltersSources
)

add_executable(expcomm_test expcomm_test.cpp)
target_link_libraries(expcomm_test MPI::MPI_CXX ${VTK_LIBRARIES})
