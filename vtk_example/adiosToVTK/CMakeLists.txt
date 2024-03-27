cmake_minimum_required (VERSION 3.14)
project (vtkcommunicator LANGUAGES C CXX)

find_package(ADIOS2)

# find_package(VTK COMPONENTS
# CommonDataModel
# )

add_executable(adiosToVTKSpecies adiosToVTKSpecies.cpp)
target_link_libraries(adiosToVTKSpecies adios2::cxx11)


add_executable(adiosToVTKCommon adiosToVTKCommon.cpp)
target_link_libraries(adiosToVTKCommon adios2::cxx11)