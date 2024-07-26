cmake_minimum_required (VERSION 3.5)
project (DrawRectangle LANGUAGES CXX)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
#find_package(glad REQUIRED)


message(${OPENGL_LIBRARY})
#message(${GLAD_LIBRARY})

add_executable(BasicTexture BasicTexture.cpp)
target_link_libraries(BasicTexture PUBLIC glfw ${OPENGL_LIBRARY})
