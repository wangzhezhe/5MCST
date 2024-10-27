cmake_minimum_required (VERSION 3.2)
project (vtktovtkm LANGUAGES C CXX)


find_package(MPI REQUIRED)

find_package(VTK COMPONENTS
IOLegacy
IOXML
IOImage
CommonDataModel
FiltersSources
FiltersCore
ParallelMPI
)

# do not use the vtkm under the vtk
# using the vtkm as a separate one
find_package(VTKm REQUIRED QUIET)
set(VTKm_LIBRARIES vtkm::cont vtkm::filter vtkm::source vtkm::io)
#message(STATUS "VTKm Libraries ${VTKm_LIBRARIES}")

add_executable(test test.cpp)
target_link_libraries(test ${VTK_LIBRARIES} ${VTKm_LIBRARIES} MPI::MPI_CXX MPI::MPI_C)

# add_executable(test_nomarshal test_nomarshal.cpp)
# target_link_libraries(test_nomarshal ${VTK_LIBRARIES} MPI::MPI_CXX MPI::MPI_C)
