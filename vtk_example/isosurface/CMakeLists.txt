cmake_minimum_required (VERSION 3.14)
project (vtkcommunicator LANGUAGES C CXX)

find_package(VTK COMPONENTS
CommonDataModel
FiltersCore
IOImage
IOXML
IOLegacy
)

add_executable(test_isosurface test_isosurface.cpp)
target_link_libraries(test_isosurface ${VTK_LIBRARIES})

add_executable(test_isosurfaceself test_isosurfaceself.cpp vtkMarchingCubesSelf.cxx)
target_link_libraries(test_isosurfaceself ${VTK_LIBRARIES})