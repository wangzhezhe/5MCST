cmake_minimum_required (VERSION 3.2)
project (vtkoctree LANGUAGES CXX)

find_package(VTK COMPONENTS
IOLegacy
IOXML
)

add_executable(vtkoctree octree.cpp)
target_link_libraries(vtkoctree ${VTK_LIBRARIES})