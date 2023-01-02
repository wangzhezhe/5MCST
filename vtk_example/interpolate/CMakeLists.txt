cmake_minimum_required (VERSION 3.14)
project (interpolate LANGUAGES C CXX)

find_package(VTK COMPONENTS
CommonDataModel
FiltersCore
IOImage
IOXML
IOLegacy
)

add_executable(vtkprob_example vtkprob_example.cpp)
target_link_libraries(vtkprob_example ${VTK_LIBRARIES})
