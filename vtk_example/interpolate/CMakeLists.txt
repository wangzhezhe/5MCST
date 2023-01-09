cmake_minimum_required (VERSION 3.14)
project (interpolate LANGUAGES C CXX)

find_package(VTK COMPONENTS
CommonDataModel
FiltersCore
FiltersGeneral
FiltersPoints
IOImage
IOXML
IOLegacy
)

add_executable(vtkprob_example_cell vtkprob_example_cell.cpp)
target_link_libraries(vtkprob_example_cell ${VTK_LIBRARIES})

add_executable(vtk_different_interp_kernel vtk_different_interp_kernel.cpp)
target_link_libraries(vtk_different_interp_kernel ${VTK_LIBRARIES})
