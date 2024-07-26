cmake_minimum_required (VERSION 3.5)
project (BasicGlew LANGUAGES CXX)

find_package(OpenGL REQUIRED)

find_package(glfw3 REQUIRED)

add_executable(testglew testglew.cpp)
target_link_libraries(testglew PUBLIC glfw OpenGL)