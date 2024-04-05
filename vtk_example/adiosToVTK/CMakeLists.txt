cmake_minimum_required (VERSION 3.14)
project (vtkcommunicator LANGUAGES C CXX)

find_package(ADIOS2)
find_package(VTK COMPONENTS
CommonDataModel
FiltersCore
IOLegacy
IOXML
)

# find_package(VTK COMPONENTS
# CommonDataModel
# )

add_executable(adiosToVTKSpecies adiosToVTKSpecies.cpp)
target_link_libraries(adiosToVTKSpecies adios2::cxx11 ${VTK_LIBRARIES})


add_executable(adiosToRawCommon adiosToRawCommon.cpp)
target_link_libraries(adiosToRawCommon adios2::cxx11 ${VTK_LIBRARIES})


add_executable(adiosToVTKBent adiosToVTKBent.cpp)
target_link_libraries(adiosToVTKBent adios2::cxx11 ${VTK_LIBRARIES})