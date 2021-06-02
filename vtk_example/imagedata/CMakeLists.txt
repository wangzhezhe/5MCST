cmake_minimum_required (VERSION 3.14)
project (vtkcommunicator LANGUAGES C CXX)

find_package(VTK COMPONENTS
CommonDataModel
FiltersCore
IOImage
IOXML
)

add_executable(test_imagedata test_imagedata.cpp)
target_link_libraries(test_imagedata ${VTK_LIBRARIES})