cmake_minimum_required(VERSION 3.14)
project(init_test LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)

find_package(MPI REQUIRED) 

#set (MPIPLINKDIR /home/zw241/cworkspace/build/build_mpip/lib)
#set (MPIPLINKLIB -lmpiP -lm -lbfd -liberty -lunwind)

#add_executable(init_test init_test.cpp)
#target_link_directories(init_test PUBLIC ${MPIPLINKDIR})
#target_link_libraries(init_test MPI::MPI_C ${MPIPLINKLIB})


add_executable(init_cart init_cart.cpp)
target_link_libraries(init_cart MPI::MPI_CXX)


add_executable(init_dim init_dim.cpp)
target_link_libraries(init_dim MPI::MPI_CXX)


#add_executable(mpinotifyblock mpinotifyblock.cpp)
#target_link_libraries(mpinotifyblock MPI::MPI_CXX)


add_executable(mpi_isend_recv mpi_isend_recv.cpp)
target_link_libraries(mpi_isend_recv MPI::MPI_CXX)

add_executable(mpi_isend_recv_anysrc mpi_isend_recv_anysrc.cpp)
target_link_libraries(mpi_isend_recv_anysrc MPI::MPI_CXX)

add_executable(mpi_isend_iprobe_recv mpi_isend_iprobe_recv.cpp)
target_link_libraries(mpi_isend_iprobe_recv MPI::MPI_CXX)


add_executable(mpi_isend_iprobe_irecv mpi_isend_iprobe_irecv.cpp)
target_link_libraries(mpi_isend_iprobe_irecv MPI::MPI_CXX)