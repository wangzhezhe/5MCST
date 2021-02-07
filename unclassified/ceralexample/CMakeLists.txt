cmake_minimum_required (VERSION 3.14)
project (CEREALEXAMPLE C CXX)
set (CMAKE_CXX_STANDARD 14)

#add cereal include
include_directories(./cereal/include)
add_executable (testserilize examplemarshal.cpp)
target_link_libraries (testserilize)
