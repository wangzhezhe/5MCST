cmake_minimum_required (VERSION 3.5)
project (3DTriangle LANGUAGES CXX)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
#find_package(glad REQUIRED)


message(${OPENGL_LIBRARY})
#message(${GLAD_LIBRARY})

add_executable(3DTriangle 3DTriangle.cpp)
target_link_libraries(3DTriangle PUBLIC glfw ${OPENGL_LIBRARY})
