##============================================================================
##  Copyright (c) Kitware, Inc.
##  All rights reserved.
##  See LICENSE.txt for details.
##
##  This software is distributed WITHOUT ANY WARRANTY; without even
##  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##  PURPOSE.  See the above copyright notice for more information.
##============================================================================
cmake_minimum_required(VERSION 3.12...3.15 FATAL_ERROR)
project(HelloWorklet CXX)

#Find the VTK-m package
find_package(VTKm REQUIRED QUIET)

add_executable(TestBoundsMap TestBoundsMap.cpp)
set(VTKm_LIBRARIES vtkm::cont vtkm::filter vtkm::source vtkm::vtkmdiympi vtkm::io vtkm::filter_flow)
target_link_libraries(TestBoundsMap PRIVATE ${VTKm_LIBRARIES} MPI::MPI_CXX)

