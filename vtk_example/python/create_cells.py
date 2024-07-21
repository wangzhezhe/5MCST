# this example is used to create mesh through vtk in python
# we create multiple voxel at different regions.


# TODO, recreate the block decomposition figure
# using vtk multiblock data structure
# xmin xmax use one block
# ymin ymax use one block
# zmin zmax use one block
# middle region use one block

# refer to this exmaple
# https://examples.vtk.org/site/Python/UnstructuredGrid/UGrid/

import vtk
from vtk.util import numpy_support
from vtkmodules.vtkCommonDataModel import vtkStructuredPoints
from vtkmodules.vtkCommonDataModel import (
    VTK_HEXAHEDRON,
    VTK_LINE,
    VTK_POLYGON,
    VTK_QUAD,
    VTK_TETRA,
    VTK_TRIANGLE,
    VTK_TRIANGLE_STRIP,
    VTK_VERTEX,
    vtkUnstructuredGrid,
    VTK_VOXEL
)

import os
import math

def writeDS(fname, ds) :
    writer = vtk.vtkDataSetWriter()
    writer.SetFileVersion(4)
    #writer.SetFileTypeToASCII()
    writer.SetFileTypeToBinary()
    writer.SetFileName(fname)
    writer.SetInputData(ds)
    writer.Update()
    writer.Write()


# add one central block

points = vtk.vtkPoints()
points.InsertNextPoint(0.0, 0.0, 0.0)
points.InsertNextPoint(1.0, 0.0, 0.0)
points.InsertNextPoint(0.0, 0.0, 1.0)
points.InsertNextPoint(1.0, 0.0, 1.0)

points.InsertNextPoint(0.0, 1.0, 0.0)
points.InsertNextPoint(1.0, 1.0, 0.0)
points.InsertNextPoint(0.0, 1.0, 1.0)
points.InsertNextPoint(1.0, 1.0, 1.0)

pts = [0,1,2,3,4,5,6,7]
pts_2 = [8,9,10,11,12,13,14,15]

ugrid = vtk.vtkUnstructuredGrid()
ugrid.SetPoints(points)
ugrid.InsertNextCell(VTK_VOXEL, 8, pts)

#print(ugrid)
writeDS("test_central.vtk",ugrid)

# vx regions
px=0.1
points_x = vtk.vtkPoints()
points_x.InsertNextPoint(0.0, 0.0, 0.0)
points_x.InsertNextPoint(px, 0.0, 0.0)
points_x.InsertNextPoint(0.0, 0.0, 1.0)
points_x.InsertNextPoint(px, 0.0, 1.0)

points_x.InsertNextPoint(0.0, 1.0, 0.0)
points_x.InsertNextPoint(px, 1.0, 0.0)
points_x.InsertNextPoint(0.0, 1.0, 1.0)
points_x.InsertNextPoint(px, 1.0, 1.0)

points_x.InsertNextPoint(1-px, 0.0, 0.0)
points_x.InsertNextPoint(1.0, 0.0, 0.0)
points_x.InsertNextPoint(1-px, 0.0, 1.0)
points_x.InsertNextPoint(1.0, 0.0, 1.0)

points_x.InsertNextPoint(1-px, 1.0, 0.0)
points_x.InsertNextPoint(1.0, 1.0, 0.0)
points_x.InsertNextPoint(1-px, 1.0, 1.0)
points_x.InsertNextPoint(1.0, 1.0, 1.0)

ugrid_x = vtk.vtkUnstructuredGrid()
ugrid_x.SetPoints(points_x)
ugrid_x.InsertNextCell(VTK_VOXEL, 8, pts)
ugrid_x.InsertNextCell(VTK_VOXEL, 8, pts_2)

#print(ugrid)
writeDS("test_x.vtk",ugrid_x)


points_y = vtk.vtkPoints()
points_y.InsertNextPoint(px, 0.0, 0.0)
points_y.InsertNextPoint(1-px, 0.0, 0.0)
points_y.InsertNextPoint(px, 0.0, 1.0)
points_y.InsertNextPoint(1-px, 0.0, 1.0)

points_y.InsertNextPoint(px, px, 0.0)
points_y.InsertNextPoint(1-px, px, 0.0)
points_y.InsertNextPoint(px, px, 1.0)
points_y.InsertNextPoint(1-px, px, 1.0)

points_y.InsertNextPoint(px, 1-px, 0.0)
points_y.InsertNextPoint(1-px, 1-px, 0.0)
points_y.InsertNextPoint(px, 1-px, 1.0)
points_y.InsertNextPoint(1-px, 1-px, 1.0)

points_y.InsertNextPoint(px, 1.0, 0.0)
points_y.InsertNextPoint(1-px, 1.0, 0.0)
points_y.InsertNextPoint(px, 1.0, 1.0)
points_y.InsertNextPoint(1.0-px, 1.0, 1.0)

ugrid_y = vtk.vtkUnstructuredGrid()
ugrid_y.SetPoints(points_y)
ugrid_y.InsertNextCell(VTK_VOXEL, 8, pts)
ugrid_y.InsertNextCell(VTK_VOXEL, 8, pts_2)

#print(ugrid)
writeDS("test_y.vtk",ugrid_y)


points_z = vtk.vtkPoints()
points_z.InsertNextPoint(px, px, 0.0)
points_z.InsertNextPoint(1-px, px, 0.0)
points_z.InsertNextPoint(px, px, px)
points_z.InsertNextPoint(1-px, px, px)

points_z.InsertNextPoint(px, 1-px, 0.0)
points_z.InsertNextPoint(1-px, 1-px, 0.0)
points_z.InsertNextPoint(px, 1-px, px)
points_z.InsertNextPoint(1-px, 1-px, px)

points_z.InsertNextPoint(px, px, 1-px)
points_z.InsertNextPoint(1-px, px, 1-px)
points_z.InsertNextPoint(px, px, 1.0)
points_z.InsertNextPoint(1-px, px, 1.0)

points_z.InsertNextPoint(px, 1.0-px, 1-px)
points_z.InsertNextPoint(1-px, 1-px, 1-px)
points_z.InsertNextPoint(px, 1-px, 1.0)
points_z.InsertNextPoint(1.0-px, 1-px, 1.0)

ugrid_z = vtk.vtkUnstructuredGrid()
ugrid_z.SetPoints(points_z)
ugrid_z.InsertNextCell(VTK_VOXEL, 8, pts)
ugrid_z.InsertNextCell(VTK_VOXEL, 8, pts_2)

#print(ugrid)
writeDS("test_z.vtk",ugrid_z)