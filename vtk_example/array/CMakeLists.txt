cmake_minimum_required (VERSION 3.14)
project (vtkcommunicator LANGUAGES C CXX)

find_package(VTK COMPONENTS
CommonDataModel
)

add_executable(example example.cpp)
target_link_libraries(example ${VTK_LIBRARIES})

add_executable(forloops forloops.cpp)
target_link_libraries(forloops ${VTK_LIBRARIES})

add_executable(testsoa testsoa.cpp)
target_link_libraries(testsoa ${VTK_LIBRARIES})

add_executable(testaos testaos.cpp)
target_link_libraries(testaos ${VTK_LIBRARIES})