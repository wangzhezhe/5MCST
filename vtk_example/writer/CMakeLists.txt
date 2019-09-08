cmake_minimum_required (VERSION 3.2)
project (vtkreader LANGUAGES CXX)

find_package(VTK COMPONENTS
IOLegacy
CommonDataModel
FiltersCore
IOXML
)


add_executable(writer_Poly writer_Poly.cxx)
target_link_libraries(writer_Poly ${VTK_LIBRARIES})