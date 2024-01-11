cmake_minimum_required(VERSION 3.12...3.15 FATAL_ERROR)
project(VtkmExample CXX)

#Find the VTK-m package
find_package(VTKm REQUIRED QUIET)

find_package(VTKm REQUIRED QUIET)
set(VTKm_LIBRARIES vtkm::cont vtkm::filter vtkm::source vtkm::vtkmdiympi vtkm::io vtkm::rendering)
message(STATUS "VTKm Libraries ${VTKm_LIBRARIES}")

add_executable(rumtime_vec rumtime_vec.cpp)
target_link_libraries(rumtime_vec PRIVATE ${VTKm_LIBRARIES})

add_executable(streamline_tube streamline_tube.cpp)
target_link_libraries(streamline_tube PRIVATE ${VTKm_LIBRARIES})

