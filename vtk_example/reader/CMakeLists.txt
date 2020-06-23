cmake_minimum_required (VERSION 3.2)
project (vtkreader LANGUAGES CXX)

find_package(VTK COMPONENTS
IOLegacy
IOXML
)

add_executable(reader_Poly reader_Poly.cxx)
target_link_libraries(reader_Poly ${VTK_LIBRARIES})


add_executable(reader_unstructured reader_unstructured.cxx)
target_link_libraries(reader_unstructured ${VTK_LIBRARIES})