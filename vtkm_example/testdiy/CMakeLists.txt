cmake_minimum_required(VERSION 3.12...3.15 FATAL_ERROR)
project(VtkmExample CXX)

#Find the VTK-m package
find_package(VTKm REQUIRED QUIET)
set(VTKm_LIBRARIES vtkm::cont vtkm::filter vtkm::source vtkm::vtkmdiympi vtkm::io)
message(STATUS "VTKm Libraries ${VTKm_LIBRARIES}")

add_executable(serilizedataset serilizedataset.cpp)
target_link_libraries(serilizedataset PRIVATE ${VTKm_LIBRARIES})

add_executable(serialize2 serialize2.cpp)
target_link_libraries(serialize2 PRIVATE ${VTKm_LIBRARIES})
