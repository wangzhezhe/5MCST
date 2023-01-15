import vtk
import os
import sys

import vtk as vtk

from vtkmodules.vtkCommonDataModel import vtkStructuredPoints


def writeDS(fname, ds) :
    writer = vtk.vtkDataSetWriter()
    writer.SetFileVersion(4)
    #writer.SetFileTypeToASCII()
    writer.SetFileTypeToBinary()
    writer.SetFileName(fname)
    writer.SetInputData(ds)
    writer.Update()
    writer.Write()

def readDS(fname) :
    reader = vtk.vtkDataSetReader()
    reader.SetFileName(fname)
    reader.ReadAllVectorsOn()
    reader.ReadAllScalarsOn()
    reader.Update()
    ds = reader.GetOutput()
    return ds

if __name__ == "__main__":
    filename = sys.argv[1]
    fieldarray= "ground_truth"
    ds = readDS(filename)
    #print(ds)
    pointArray = ds.GetPointData().GetArray(fieldarray)
    #create new data set
    vol = vtkStructuredPoints()
    vol.SetDimensions(496, 832, 832)
    vol.SetOrigin(0, 0, 0)

    vol.GetPointData().AddArray(pointArray)

    print(vol)
    writeDS("beetle_496_832_832.vtk",vol)

