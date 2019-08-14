cmake_minimum_required (VERSION 3.2)
project(FooBar LANGUAGES CXX)

find_package(FooBar REQUIRED)

message("FOOBAR_INCLUDE_DIRS:${FOOBAR_INCLUDE_DIRS}")
message("FOOBAR_LIBRARIES:${FOOBAR_LIBRARIES}")

add_executable(main main.cpp)
target_include_directories(main PUBLIC ${FOOBAR_INCLUDE_DIRS})
target_link_libraries(main FooBar::foo)



