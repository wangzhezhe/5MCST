import vtk
import os
import sys
from vtk.util import numpy_support
import numpy as np
import math
from vtkmodules.vtkCommonDataModel import vtkStructuredPoints

def readDS(fname) :
    reader = vtk.vtkXMLMultiBlockDataReader()
    reader.SetFileName(fname)
    reader.Update()
    ds = reader.GetOutput()
    return ds

if __name__ == "__main__":

    filename = sys.argv[1]
    ds=readDS(filename)
    #print(ds)

    
    num_blocks = ds.GetNumberOfBlocks()
    print("num_blocks",num_blocks)

    # get first block
    data_block = ds.GetBlock(0)
    
    # this data block is unstructured data set
    print("data_block")
    print(data_block)

    # save the coordinates for points
    points = data_block.GetPoints()
    num_points = data_block.GetNumberOfPoints()
    print(points)
    print("num_points are",num_points)
    # The data model for vtkPoints is an array of vx-vy-vz triplets accessible by (point or cell) id.
    # print(points.GetPoint(0))
    # go through each point and store the value into the dedicated array
    coordsx=[]
    coordsy=[]
    coordsz=[]
    
    # extracting coordinates
    for pindex in range(0,num_points,1):
        p=points.GetPoint(pindex)
        coordsx.append(p[0])
        coordsy.append(p[1])
        coordsz.append(p[2])

    # variables
    ux=[]
    uy=[]
    uz=[]
    p=[]

