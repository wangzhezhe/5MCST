import vtk
import os
import sys

import numpy as np
import vtk as vtk
from vtk.util import numpy_support

from vtkmodules.vtkCommonDataModel import vtkStructuredPoints


# this is 5.1 anyway
# we can use structured_dataset to write into specific version
def writeDS(fname, ds) :
    writer = vtk.vtkDataSetWriter()
    writer.SetFileTypeToBinary()
    writer.SetFileName(fname)
    writer.SetInputData(ds)
    writer.Update()
    writer.Write()

# we can set the version of the writter if we 
# use the dedicated writer to write the data
def writeStructuredDs(fname, ds):
    writer = vtk.vtkStructuredPointsWriter()
    writer.SetFileName(fname)
    writer.SetFileVersion(42)
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

    #creating sample data set
    xdim=10
    ydim=20
    zdim=30
    structured_dataset = vtkStructuredPoints()
    structured_dataset.SetDimensions(xdim, ydim, zdim)
    structured_dataset.SetOrigin(0, 0, 0)

    point_data_raw = np.arange(0,xdim*ydim*zdim,1,dtype=int)

    print(point_data_raw.shape)

    vtkarray = numpy_support.numpy_to_vtk(point_data_raw)
    # the default array name is unknown
    vtkarray.SetName("pointArray")
   
    structured_dataset.GetPointData().AddArray(vtkarray)
    structured_dataset.GetPointData().SetActiveScalars("pointArray")

    # this output use the most general datset writer which is 5.1 version
    writeDS("test_raw_10_20_30.vtk",structured_dataset)
    # this output use the specific structured point data set writer
    # which can be set as the 4.2 version
    writeStructuredDs("test_42_raw_10_20_30.vtk",structured_dataset)

    # get data from the data set

    numpy_array = structured_dataset.GetPointData().GetArray("pointArray")

    # we can then do some operation based on the numpy array
