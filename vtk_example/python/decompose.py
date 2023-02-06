# this script first resample the data into the 
# didicated dims and then decompose the files
# the original dims of the file might not be suitable
# to be divided into equal sizes.

# we can either set the small block size as fixed number
# or set the total dims as the fixed number

import vtk
from vtk.util import numpy_support
from vtkmodules.vtkCommonDataModel import vtkStructuredPoints

import os
import math
import numpy as np

def writeDS(fname, ds) :
    writer = vtk.vtkDataSetWriter()
    writer.SetFileVersion(4)
    #writer.SetFileTypeToASCII()
    writer.SetFileTypeToBinary()
    writer.SetFileName(fname)
    writer.SetInputData(ds)
    writer.Update()
    writer.Write()


def decompose_by_blocknum(dataset,num_blocks):
    pt_dims = dataset.GetDimensions()
    for i in range(3):
        # we need to consider the overlapping
        if ((pt_dims[i]+1)%num_blocks[i])!=0:
            print("error: (point dim + 1) should be divided by number of blocks", pt_dims[i]+1, num_blocks[i])
            exit(-1)

    # number of points in each block
    dx = int((pt_dims[0]+1)/num_blocks[0])
    dy = int((pt_dims[1]+1)/num_blocks[1])
    # for the 2d case, we set dz to 1 direactly
    # other wise, the extent array will be out of bounuds
    dz = 1
    cnt = 0
    fnames = []
    for bi in range(num_blocks[0]) :
        for bj in range(num_blocks[1]) :
            for bk in range(num_blocks[2]) : 
                # print bounudry for each decomposed block
                # !! the value set into the extractVOI
                # is actaully the extent of the point
                # be careful about this boundy case
                # do not exceeds the outter 
                x0 = bi*dx-1
                if x0<0:
                    x0=0
                x1 = x0+dx-1
                y0 = bj*dy-1
                if y0<0:
                    y0=0
                y1 = y0+dy-1
                z0 = bk*dz-1
                if z0<0:
                    z0=0
                z1 = z0+dz-1
                #print the block id and its boundry
                #this filter requires the extent instead of the point dims
                print((bi,bj,bk), (x0,x1), (y0,y1), (z0,z1))

                eg = vtk.vtkExtractVOI()
                voi = (x0,x1, y0,y1, z0,z1)
                eg.SetVOI(voi)
                eg.SetSampleRate(1,1,1)
                eg.SetInputData(dataset)
                eg.Update()
                out = eg.GetOutput()
                blockNm = './output.%03d.vtk'%(cnt)
                writeDS(blockNm, out)
                cnt = cnt+1
                # record file names
                fnames.append(blockNm)                

    #make .visit file
    #this can be used as the integrated file to load the vtk data
    f = open('%s.visit' % "output", 'w')
    f.write('!NBLOCKS %d\n' % cnt)
    for x in fnames :
        f.write(x + '\n')
    f.close()              

if __name__ == "__main__":

    # creating a synthetic data for testing
    # or loaing other dataset as needed
    # for simplicity, we just set 2d data
    # there are 99 points for each dim of the data set
    xdim=99
    ydim=99
    zdim=1
    structured_dataset = vtkStructuredPoints()
    structured_dataset.SetDimensions(xdim, ydim, zdim)
    structured_dataset.SetOrigin(0, 0, 0)
    structured_dataset.SetSpacing(1, 1, 1)
    # the extent is 0 98 0 98 0 98 in this case


    # add a point value to the grid, there are 100*100 points
    point_array=[]
    for i in range(0,xdim):
        for j in range(0,ydim):
            v = math.sqrt(pow((i*1.0-xdim/2.0),2)+pow((j*1.0-ydim/2.0), 2))
            point_array.append(v)

    
    vtkarray = numpy_support.numpy_to_vtk(np.array(point_array))
    vtkarray.SetName("point_value")   
    structured_dataset.GetPointData().AddArray(vtkarray)

    writeDS ("temp_testdata.vtk", structured_dataset)
    print(structured_dataset)

    # decompse the data into several small blocks
    # we can resample the data firstly if the input dims is 
    # a bad number for division
    
    # try to decompose the data into 2 2 1 at each dim
    decomp_num=[2,2,1]
    decompose_by_blocknum(structured_dataset, decomp_num)
