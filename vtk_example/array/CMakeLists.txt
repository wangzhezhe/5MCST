cmake_minimum_required (VERSION 3.14)
project (vtkcommunicator LANGUAGES C CXX)

find_package(VTK COMPONENTS
CommonDataModel
)

add_executable(example example.cpp)
target_link_libraries(example ${VTK_LIBRARIES})
