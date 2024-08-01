# the bounds is inconvenient to handle
# we could extract data and set spacing as 1
# and the origin as zero

import sys
from vtk.util import numpy_support
import numpy as np
import vtk
import os

def readDS(fname) :
   reader = vtk.vtkDataSetReader()
   reader.SetFileName(fname)
   reader.ReadAllVectorsOn()
   reader.ReadAllScalarsOn()
   reader.Update()
   ds = reader.GetOutput()
   return ds

def writeDS(fname, ds) :
    writer = vtk.vtkDataSetWriter()
    writer.SetFileVersion(4)
    #writer.SetFileTypeToASCII()
    writer.SetFileTypeToBinary()
    writer.SetFileName(fname)
    writer.SetInputData(ds)
    writer.Update()
    writer.Write()

if __name__ == "__main__":
    file_name="/Users/zhewang/DataSets/DataSetForParticleAdvectionBlockAssignment/redsea/resampled_redsea.vtk"
    ds=readDS(file_name)

    #extract the velocity field

    #create a new data set
    arrayName= "velocity"

    pointArray = ds.GetPointData().GetArray(arrayName)

    print(pointArray)

    vol = vtk.vtkStructuredPoints()
    vol.SetDimensions(500, 500, 50)
    vol.SetOrigin(0, 0, 0)

    vol.GetPointData().AddArray(pointArray)

    print(vol)
    writeDS("redsea_clip_0.vtk",vol)