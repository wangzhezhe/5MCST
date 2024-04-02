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
        #print(p)
        coordsx.append(float(p[0]))
        coordsy.append(float(p[1]))
        coordsz.append(float(p[2]))
    
    # this is the 2d data set, the z dim is same for all data
    # print(coordsx)
    # print(coordsy)
    # print(coordsz)

    # variables
    ux=[]
    uy=[]
    uz=[]

    point_array_p = data_block.GetPointData().GetArray("p")

    
    # get the numpy data
    point_nparray_p = numpy_support.vtk_to_numpy(point_array_p)
    # convert to double
    point_nparray_p = np.array(point_nparray_p,dtype=np.double)

    var_p_size=point_nparray_p.size
    print(var_p_size)

    # get the U array
    point_array_U = data_block.GetPointData().GetArray("U")

    #print(point_array_U)
    #print("point_array_U tuples:",point_array_U.GetNumberOfTuples())
    num_tuples = point_array_U.GetNumberOfTuples()
    for t in range(0,num_tuples,1):
        temp_tuple=point_array_U.GetTuple(t)
        ux.append(float(temp_tuple[0]))
        uy.append(float(temp_tuple[1]))
        uz.append(float(temp_tuple[2]))



    import adios2
    # refer to https://github.com/ornladios/ADIOS2/blob/master/examples/hello/bpWriter/bpWriter.py
    adios = adios2.Adios(config_file=None)
    nsteps=1
    bpIO = adios.declare_io("BPFile_N2N")
    bpIO.set_engine("BPFile")
    a = bpIO.adios()
    # ADIOS output stream
    with adios2.Stream(bpIO, "airfoil_rot_30_slice_60.bp", "w") as fh:
        # name, data, shape, start, count
        # write variable
        fh.write("CoordinateX", coordsx, [num_points], [0], [num_points])
        fh.write("CoordinateY", coordsy, [num_points], [0], [num_points])
        fh.write("CoordinateZ", coordsz, [num_points], [0], [num_points])

        fh.write("Ux", coordsx, [num_points], [0], [num_points])
        fh.write("Uy", coordsy, [num_points], [0], [num_points])
        fh.write("Uz", coordsz, [num_points], [0], [num_points])

        fh.write("pressure", point_nparray_p, [var_p_size], [0], [var_p_size])

    # TODO, adding connectivity and offset as needed
    # https://github.com/wangzhezhe/Gorilla/blob/master/test/BlockData/test_vtkexplicitzero.cpp