cmake_minimum_required (VERSION 3.5)
project (DrawTrangle LANGUAGES CXX)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
#find_package(glad REQUIRED)


message(${OPENGL_LIBRARY})
#message(${GLAD_LIBRARY})

add_executable(drawTrangle drawTrangle.cpp)
target_link_libraries(drawTrangle PUBLIC glfw ${OPENGL_LIBRARY})

add_executable(sampleTriangle sampleTriangle.cpp)
target_link_libraries(sampleTriangle PUBLIC glfw ${OPENGL_LIBRARY})