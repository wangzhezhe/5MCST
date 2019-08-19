cmake_minimum_required(VERSION 3.15)
project(CUDAEXP LANGUAGES C CXX CUDA)

# the shared library of the cuda should be the static
# provide the cuda compiler: cmake -DCMAKE_CUDA_COMPILER=/usr/local/cuda-10.0/bin/nvcc ..

set_source_files_properties(example.cxx PROPERTIES LANGUAGE "CUDA")
add_executable(example example.cxx)
set_target_properties(example PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

