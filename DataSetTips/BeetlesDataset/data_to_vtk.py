import struct
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


def read_unsigned_chars(filename):
    with open(filename, "rb") as f:
        # the length of unsigned short is two byte
        bit_size_x=f.read(2)
        usigned_short_x=struct.unpack('H', bit_size_x)[0]
        bit_size_y=f.read(2)
        usigned_short_y=struct.unpack('H', bit_size_y)[0]
        bit_size_z=f.read(2)
        usigned_short_z=struct.unpack('H', bit_size_z)[0]

        print("data size",usigned_short_x,usigned_short_y,usigned_short_z)

        # write all data elements
        data = []
        for z in range(0,usigned_short_z,1):
            for y in range(0,usigned_short_y,1):
                for x in range(0,usigned_short_x,1):
                    bit_size_data=f.read(2)
                    data.append(struct.unpack('H', bit_size_data)[0])

    print(len(data))

    # output data as a vtk file
    xdim=usigned_short_x
    ydim=usigned_short_y
    zdim=usigned_short_z
    structured_dataset = vtkStructuredPoints()
    structured_dataset.SetDimensions(xdim, ydim, zdim)
    structured_dataset.SetOrigin(0, 0, 0)

    # the default array name is unknown

    vtkarray = numpy_support.numpy_to_vtk(np.array(data))
    vtkarray.SetName("data")
    vtkarray.SetNumberOfComponents(1)

    print(vtkarray)
   
    structured_dataset.GetPointData().AddArray(vtkarray)
    # remember to set the activeScalar field to make sure the 
    # output data field can be detected by paraview
    structured_dataset.GetPointData().SetActiveScalars("data")
    writeStructuredDs("stagbeetle.vtk",structured_dataset)




if __name__ == "__main__":
    read_unsigned_chars("/Users/zhewang/Downloads/stagbeetle832x832x494.dat")