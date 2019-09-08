cmake_minimum_required (VERSION 3.2)
project (Tutorial LANGUAGES CXX)

OPTION (IFSTATIC ON)

if(IFSTATIC)
  message("static on")
  add_library(MathFunctions STATIC ${Tutorial_SOURCE_DIR}/util/sqrt.cpp)
  target_include_directories(MathFunctions PUBLIC ./util)

else()
  message("static off")
  add_library(MathFunctions q ${Tutorial_SOURCE_DIR}/util/sqrt.cpp)
  target_include_directories(MathFunctions PUBLIC ./util)
endif()

# add the executable
add_executable(Tutorial main.cpp)
target_link_libraries (Tutorial MathFunctions)
