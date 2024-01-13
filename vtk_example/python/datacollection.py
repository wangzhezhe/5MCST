import sys
from vtk.util import numpy_support
import numpy as np
import vtk

# associated infor about the vtkPartitionedDataSetsCollection
# https://discourse.paraview.org/t/transition-from-multiblock-to-partitioned-datasets-in-vtk-and-paraview-a-complete-guide/12061

def readDS(fname) :
   reader = vtk.vtkDataSetReader()
   reader.SetFileName(fname)
   reader.ReadAllVectorsOn()
   reader.ReadAllScalarsOn()
   reader.Update()
   ds = reader.GetOutput()
   return ds

def createDS(id):
    structured_dataset = vtk.vtkStructuredPoints()
    structured_dataset.SetDimensions(10, 10, 1)
    structured_dataset.SetOrigin(10*id+1, 0, 0)
    point_data_raw=np.arange(100)
    vtkarray = numpy_support.numpy_to_vtk(point_data_raw)
    # the default array name is unknown
    vtkarray.SetNumberOfComponents(1)
    vtkarray.SetName("pointArray")
    structured_dataset.GetPointData().AddArray(vtkarray)
    structured_dataset.GetPointData().SetActiveScalars("pointArray")
    return structured_dataset

# using PartitionedDataSetCollection
grouper = vtk.vtkGroupDataSetsFilter()
grouper.SetOutputTypeToPartitionedDataSetCollection()

for id in range(0,3,1):
    #load the vtk file from data sets on disk
    #ds=readDS(file_name)
    #we use dummy file here for testing
    ds = createDS(id)
    grouper.AddInputDataObject(ds)


grouper.Update()
#print(grouper.GetOutput())
w = vtk.vtkXMLPartitionedDataSetCollectionWriter()
w.SetInputData(grouper.GetOutput())
w.SetFileName("test_output_partition_collection_data.vtpd")
w.Write()