import sys
from vtk.util import numpy_support
import numpy as np
import vtk
import os
from paraview.simple import *

def readDS(fname) :
   reader = vtk.vtkDataSetReader()
   reader.SetFileName(fname)
   reader.ReadAllVectorsOn()
   reader.ReadAllScalarsOn()
   reader.Update()
   ds = reader.GetOutput()
   return ds

def writeStructuredDs(fname, ds):
    writer = vtk.vtkStructuredPointsWriter()
    writer.SetFileName(fname)
    writer.SetFileVersion(42)
    writer.SetInputData(ds)
    writer.Update()
    writer.Write()  

if __name__ == "__main__":
    file_name="/Users/zhewang/DataSets/redsea_raw/redsea_raw_ens10.vtk"
    ds=readDS(file_name)

    # extract data array, velocityMagnitude
    print(ds)

    dimx=500
    dimy=500
    dimz=50

    # write it to dedicated files
    # using extract subset
    id_slice = 10

    z_start=dimz-id_slice-1
    z_end=z_start

    directory = "slice_10_member"
    if not os.path.exists(directory):
        # Create the directory
        os.makedirs(directory)

    #using extracting subset filter

    # Get the extracted subset
    extractSubset1 = ExtractSubset(Input=ds,VOI = [0, 499, 0, 499, z_start, z_end])
    print(extractSubset1)
    
    output_fname = directory+"/ens_10.vtk"
    writeStructuredDs(output_fname,extractSubset1)
