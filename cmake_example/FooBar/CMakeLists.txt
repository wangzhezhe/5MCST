cmake_minimum_required(VERSION 3.2)
project(FooBar LANGUAGES CXX)

set(FOOBAR_MAJOR_VERSION 0)
set(FOOBAR_MINOR_VERSION 1)
set(FOOBAR_PATCH_VERSION 0)
set(FOOBAR_VERSION
  ${FOOBAR_MAJOR_VERSION}.${FOOBAR_MINOR_VERSION}.${FOOBAR_PATCH_VERSION})


# print some messages for debugging
message("PROJECT_SOURCE_DIR:${PROJECT_SOURCE_DIR}")
message("PROJECT_BINARY_DIR:${PROJECT_BINARY_DIR}")
message("CMAKE_INSTALL_PREFIX:${CMAKE_INSTALL_PREFIX}")
message("${CMAKE_ROOT}:${CMAKE_ROOT}")

# set up include-directories
include_directories(
  "${PROJECT_SOURCE_DIR}"   # to find foo/foo.h
  "${PROJECT_BINARY_DIR}")  # to find foo/config.h

# add intall lib into the rpath

# set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# Add sub-directories
add_subdirectory(foo)
add_subdirectory(bar)

set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(FooBarConfig.cmake.in FooBarConfig.cmake @ONLY)
configure_file(FooBarConfigVersion.cmake.in FooBarConfigVersion.cmake @ONLY)

export(TARGETS foo bar
  FILE "${PROJECT_BINARY_DIR}/FooBarTargets.cmake")
