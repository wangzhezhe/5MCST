#!/bin/bash
set -x
set -e

module load gcc cuda cmake git-lfs ninja

COMPILER_VERSION=gcc 
curr_dir=$(pwd)
host=vortex

mkdir -p test_vtkm_build
cd test_vtkm_build

#kokkos_version=develop
kokkos_version=3.7.01
cmake_build_type=Release
build_jobs=6

vtkm_version=master

if [ ! -d vtk-m ]; then
git clone -b master https://gitlab.kitware.com/vtk/vtk-m.git
fi

vtkm_src_dir=$curr_dir/test_vtkm_build/vtk-m

cd ${vtkm_src_dir}
vtkm_hash=`git log | head -1 | awk '{print $2}' | cut -c1-7`
cd ${curr_dir}/test_vtkm_build

#kokkos_src_dir=${home_dir}/ECP/Kokkos/${kokkos_version}

if [ ! -d kokkos ]; then
git clone -b $kokkos_version https://github.com/kokkos/kokkos.git
fi

kokkos_src_dir=$curr_dir/test_vtkm_build/kokkos
cd ${kokkos_src_dir}
kokkos_hash=`git log | head -1 | awk '{print $2}' | cut -c1-7`
cd ${curr_dir}/test_vtkm_build

build_dir=${curr_dir}/test_vtkm_build/${kokkos_version}_${kokkos_hash}_${vtkm_version}_${vtkm_hash}_${COMPILER_VERSION}_${cmake_build_type}

kokkos_build_dir=${build_dir}/build_kokkos/${kokkos_version}
kokkos_install_dir=${build_dir}/install_kokkos

# benchmark_src_dir=${home_dir}/ECP/benchmark/
# benchmark_build_dir=${build_dir}/build/benchmark
# benchmark_install_dir=${build_dir}/install

vtkm_build_dir=${build_dir}/build_vtkm/${vtkm_version}
vtkm_install_dir=${build_dir}/install_vtkm

# if ${checkout}; then
#   cd ${home_dir}/ECP
#   git clone https://github.com/google/benchmark.git
#   git clone https://github.com/google/googletest.git benchmark/googletest
#   git clone -b develop https://github.com/kokkos/kokkos.git ${kokkos_src_dir}
#   git clone https://github.com/kokkos/kokkos.git ${kokkos_src_dir}
#   cd ${home_dir}
# fi

# rm -rf ${benchmark_build_dir}
# cmake -S ${benchmark_src_dir} -B ${benchmark_build_dir} \
#   -DCMAKE_BUILD_TYPE=${cmake_build_type} \
#   -DCMAKE_CXX_COMPILER=${CXX_COMPILER} \
#   -DCMAKE_C_COMPILER=${C_COMPILER} \
#   -DCMAKE_INSTALL_PREFIX=${benchmark_install_dir}
# cmake --build ${benchmark_build_dir} -j10
# cmake --install ${benchmark_build_dir}

    CXX=${kokkos_src_dir}/bin/nvcc_wrapper
    sed -i 's/sm_35/sm_70/g' $CXX


if [ ! -d ${kokkos_install_dir} ]; then
rm -rf ${kokkos_build_dir}
cmake -S ${kokkos_src_dir} -B ${kokkos_build_dir} \
   -DCMAKE_BUILD_TYPE=${cmake_build_type} \
   -DCMAKE_CXX_FLAGS=-fPIC \
   -DBUILD_SHARED_LIBS=ON \
   -DKokkos_ENABLE_EXAMPLES=OFF \
   -DKokkos_ENABLE_TESTS=OFF \
   -DKokkos_ENABLE_CUDA=ON \
   -DKokkos_ENABLE_CUDA_CONSTEXPR=ON \
   -DKokkos_ENABLE_CUDA_LAMBDA=ON \
   -DKokkos_ENABLE_CUDA_LDG_INTRINSIC=ON \
   -DKokkos_ENABLE_CUDA_RELOCATABLE_DEVICE_CODE=OFF \
   -DKokkos_ENABLE_CUDA_UVM=ON \
   -DCMAKE_CXX_STANDARD=14 \
   -DCMAKE_CXX_COMPILER=g++ \
   -DCMAKE_C_COMPILER=gcc \
   -DCMAKE_INSTALL_PREFIX=${kokkos_install_dir}
 cd ${kokkos_build_dir}
make -j${build_jobs}
make install
fi

cd $curr_dir/test_vtkm_build
rm -rf ${vtkm_build_dir}
cmake -S ${vtkm_src_dir} -B ${vtkm_build_dir} -GNinja \
   -DCMAKE_BUILD_TYPE=${cmake_build_type} \
   -DVTKm_NO_DEPRECATED_VIRTUAL=ON \
   -DVTKm_ENABLE_KOKKOS=ON \
   -DVTKm_ENABLE_RENDERING=OFF \
   -DVTKm_ENABLE_BENCHMARKS=OFF \
   -DVTKm_ENABLE_CUDA=ON \
   -DVTKm_CUDA_Architecture=volta \
   -DCMAKE_CUDA_ARCHITECTURES=70 \
   -DBUILD_SHARED_LIBS=OFF \
   -DVTKm_USE_DOUBLE_PRECISION=OFF \
   -DVTKm_ENABLE_TESTING=OFF \
   -DCMAKE_CXX_STANDARD=14 \
   -DCMAKE_PREFIX_PATH=${kokkos_install_dir} \
   -DCMAKE_INSTALL_PREFIX=${vtkm_install_dir} \
   -DCMAKE_CXX_COMPILER=g++ \
   -DCMAKE_C_COMPILER=gcc \
   -DKokkos_DIR=${kokkos_install_dir}/lib64/cmake/Kokkos \
   -DVTKm_ENABLE_KOKKOS=ON \
   -DKokkos_COMPILE_LAUNCHER=${kokkos_install_dir}/bin/kokkos_launch_compiler \
   -DKokkos_NVCC_WRAPPER=${kokkos_install_dir}/bin/nvcc_wrapper 

   #-Dbenchmark_DIR=${benchmark_install_dir}/lib64/cmake/benchmark

cd ${vtkm_build_dir}
cmake --build . -j${build_jobs}
