cmake_minimum_required (VERSION 3.2)
project (debug LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)


add_executable(test test.cpp)
# there is CASE1 if we add this

# this means `#define CASE1` in the source code
# target_compile_definitions(test PRIVATE CASE1)

# there is compiling error if we do not add LEN
# adding LEN here is same with declare `#define LEN 1` in the source code
# target_compile_definitions(test PRIVATE CASE1 LEN=1)

# refer to this 
# https://stackoverflow.com/questions/9017573/define-preprocessor-macro-through-cmake