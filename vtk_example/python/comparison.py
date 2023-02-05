# load two image data and compare their difference

import vtk
import os
import sys
from vtk.util import numpy_support
import numpy as np
import math
from vtkmodules.vtkCommonDataModel import vtkStructuredPoints

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

    filename1 = sys.argv[1]
    filename2 = sys.argv[2]

    ds1 = readDS(filename1)
    ds2 = readDS(filename2)

    cellArrayName= "cross_prob"

    cellArray1 = ds1.GetCellData().GetArray(cellArrayName)
    cellArray2 = ds2.GetCellData().GetArray(cellArrayName)

    nparray1 = numpy_support.vtk_to_numpy(cellArray1)
    nparray2 = numpy_support.vtk_to_numpy(cellArray2)

    print(nparray1)
    print(nparray2)

    diff_list = []
    max_diff = 0
    max_idx = 0
    for idx, _ in enumerate(nparray1):
        diff = math.fabs(nparray1[idx]-nparray2[idx])
        diff_list.append(diff)
        if diff>max_diff:
            max_diff = diff
            max_idx = idx

    print(np.array(diff_list))
    print("max ", max_diff)
    print("max idx ", max_idx)
    
    # the grid of the dataset
    xdim=240
    ydim=121
    zdim=1
    structured_dataset = vtkStructuredPoints()
    structured_dataset.SetDimensions(xdim, ydim, zdim)
    structured_dataset.SetOrigin(0, 0, 0)

    # the default array name is unknown

    vtkarray = numpy_support.numpy_to_vtk(np.array(diff_list))
    vtkarray.SetName("diff")

    print(vtkarray)
   
    structured_dataset.GetCellData().AddArray(vtkarray)
    writeStructuredDs("wind_cross_prob_diff.vtk",structured_dataset)