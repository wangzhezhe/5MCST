cmake_minimum_required (VERSION 3.5)
project (drawpoints LANGUAGES CXX)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
#find_package(glad REQUIRED)


message(${OPENGL_LIBRARY})
#message(${GLAD_LIBRARY})

add_executable(drawpoints drawpoints.cpp)
target_link_libraries(drawpoints PUBLIC glfw ${OPENGL_LIBRARY})
