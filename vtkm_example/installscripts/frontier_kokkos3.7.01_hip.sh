#!/bin/bash
set -x
set -e

module load rocm cmake git-lfs ninja

curr_dir=$(pwd)

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


vtkm_build_dir=${build_dir}/build_vtkm/${vtkm_version}
vtkm_install_dir=${build_dir}/install_vtkm


if [ ! -d ${kokkos_install_dir} ]; then
rm -rf ${kokkos_build_dir}
cmake -S ${kokkos_src_dir} -B ${kokkos_build_dir} \
   -DCMAKE_BUILD_TYPE=${cmake_build_type} \
   -DBUILD_SHARED_LIBS=ON \
   -DKokkos_ARCH_VEGA90A=ON \
   -DCMAKE_CXX_COMPILER=hipcc \
   -DKokkos_ENABLE_HIP=ON \
   -DKokkos_ENABLE_SERIAL=ON \
   -DKokkos_ENABLE_HIP_RELOCATABLE_DEVICE_CODE=OFF \
   -DCMAKE_INSTALL_PREFIX=${kokkos_install_dir} \
   -DCMAKE_CXX_FLAGS="--amdgpu-target=gfx90a"

cd ${kokkos_build_dir}
make -j${build_jobs}
make install
fi

cd $curr_dir/test_vtkm_build
rm -rf ${vtkm_build_dir}
cmake -S ${vtkm_src_dir} -B ${vtkm_build_dir} -GNinja \
   -DCMAKE_BUILD_TYPE=${cmake_build_type} \
   -DBUILD_SHARED_LIBS=OFF \
   -DVTKm_USE_DEFAULT_TYPES_FOR_ASCENT=ON \
   -DVTKm_USE_DOUBLE_PRECISION=ON \
   -DVTKm_NO_DEPRECATED_VIRTUAL=ON \
   -DVTKm_USE_64BIT_IDS=OFF \
   -DCMAKE_INSTALL_PREFIX=${vtkm_install_dir} \
   -DVTKm_ENABLE_MPI=ON \
   -DVTKm_ENABLE_OPENMP=ON \
   -DVTKm_ENABLE_LOGGING=ON \
   -DVTKm_ENABLE_RENDERING=OFF \
   -DVTKm_ENABLE_KOKKOS=ON \
   -DVTKm_ENABLE_TESTING=OFF \
   -DCMAKE_HIP_ARCHITECTURES=gfx90a \
   -DCMAKE_PREFIX_PATH=${kokkos_install_dir} \
   -DCMAKE_CXX_COMPILER=hipcc -DCMAKE_C_COMPILER=hipcc

cd ${vtkm_build_dir}
cmake --build . -j${build_jobs}
