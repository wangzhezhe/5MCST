cmake_minimum_required (VERSION 3.2)
project (vtkreader LANGUAGES CXX)

find_package(VTK COMPONENTS
vtkIOLegacy
)

add_executable(reader_Poly reader_Poly.cxx)
target_link_libraries(reader_Poly ${VTK_LIBRARIES})