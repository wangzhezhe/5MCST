cmake_minimum_required (VERSION 3.14)
project (Streamlines LANGUAGES C CXX)

find_package(VTK COMPONENTS 
  CommonColor
  CommonCore
  CommonDataModel
  FiltersCore
  FiltersFlowPaths
  FiltersSources
  IOParallel
)

add_executable(streamline_example1 streamline_example1.cpp)
target_link_libraries(streamline_example1 ${VTK_LIBRARIES})

add_executable(streamline_exampleKitchen streamline_exampleKitchen.cpp)
target_link_libraries(streamline_exampleKitchen ${VTK_LIBRARIES})

add_executable(mystreamline_example mystreamline_example.cpp myvtkStreamTracer.cxx)
target_link_libraries(mystreamline_example ${VTK_LIBRARIES})