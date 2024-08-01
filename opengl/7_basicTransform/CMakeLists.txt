cmake_minimum_required (VERSION 3.5)
project (BasicTransform LANGUAGES CXX)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
#find_package(glad REQUIRED)


message(${OPENGL_LIBRARY})
#message(${GLAD_LIBRARY})

add_executable(basicTransform basicTransform.cpp)
target_link_libraries(basicTransform PUBLIC glfw ${OPENGL_LIBRARY})
