# refer to https://examples.vtk.org/site/Python/Visualization/StreamLines/
import vtk as vtk
import sys
from vtkmodules.vtkFiltersSources import vtkLineSource


def readDS(fname) :
    reader = vtk.vtkDataSetReader()
    reader.SetFileName(fname)
    reader.ReadAllVectorsOn()
    reader.ReadAllScalarsOn()
    reader.Update()
    ds = reader.GetOutput()
    return ds

if __name__ == "__main__":
    if len(sys.argv)!=2:
        print("pyscript <input vtk file>")
        exit(0)

    filename = sys.argv[1]
    ds = readDS(filename)
    #print(ds)

    seeds = vtkLineSource()
    seeds.SetResolution(10)
    seeds.SetPoint1(0.08, 2.50, 0.71)
    seeds.SetPoint2(0.08, 4.50, 0.71)


    # call the stream trace filter
    stream_tracer = vtk.vtkStreamTracer()
    stream_tracer.SetInputData(ds)
    stream_tracer.SetSourceConnection(seeds.GetOutputPort())
    stream_tracer.SetMaximumPropagation(100)
    stream_tracer.SetInitialIntegrationStep(0.01)
    stream_tracer.SetIntegrationDirectionToBoth()

    # write file out for checking results
    # poly_writer=vtk.vtkPolyDataWriter()
    # poly_writer.SetFileName("./streamline_output.vtk")
    # poly_writer.SetInputConnection(stream_tracer.GetOutputPort())
    # poly_writer.Update()

    # the whole pipeline will be executed after calling Update
    stream_tracer.Update()
    trace_data = stream_tracer.GetOutput()

    # TODO how to find the da

    print(trace_data)