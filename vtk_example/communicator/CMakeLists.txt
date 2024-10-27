cmake_minimum_required (VERSION 3.14)
project (vtkcommunicator LANGUAGES C CXX)

find_package(MPI REQUIRED)

find_package(VTK COMPONENTS
CommonDataModel
ParallelMPI
FiltersSources
FiltersCore
IOImage
IOXML
)

add_executable(expcomm_test expcomm_test.cpp)
target_link_libraries(expcomm_test MPI::MPI_CXX ${VTK_LIBRARIES})


add_executable(marshal_unmarshal marshal_unmarshal.cpp)
target_link_libraries(marshal_unmarshal MPI::MPI_CXX ${VTK_LIBRARIES})
