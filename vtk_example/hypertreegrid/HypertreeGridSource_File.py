#!/usr/bin/env python
import vtk
# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersGeneral import vtkShrinkFilter
from vtkmodules.vtkFiltersHyperTree import vtkHyperTreeGridToUnstructuredGrid
from vtkmodules.vtkFiltersSources import vtkHyperTreeGridSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)

def writeUSDS(fname, ds) :
    writer = vtk.vtkXMLUnstructuredGridWriter()
    writer.SetFileName(fname)
    writer.SetInputData(ds)
    writer.Update()
    writer.Write()

def writeHTDS(fname, ds) :
    writer = vtk.vtkXMLHyperTreeGridWriter()
    writer.SetFileName(fname)
    writer.SetInputData(ds)
    writer.Update()
    writer.Write()

def main():
    colors = vtkNamedColors()
    # Create hyper tree grid source

    descriptor = 'RRR .R. .RR ..R ..R .R.|R.......................... ' \
                 '........................... ........................... ' \
                 '.............R............. ....RR.RR........R......... ' \
                 '.....RRRR.....R.RR......... ........................... ' \
                 '........................... ' \
                 '...........................|........................... ' \
                 '........................... ........................... ' \
                 '...RR.RR.......RR.......... ........................... ' \
                 'RR......................... ........................... ' \
                 '........................... ........................... ' \
                 '........................... ........................... ' \
                 '........................... ........................... ' \
                 '............RRR............|........................... ' \
                 '........................... .......RR.................. ' \
                 '........................... ........................... ' \
                 '........................... ........................... ' \
                 '........................... ........................... ' \
                 '........................... ' \
                 '...........................|........................... ' \
                 '...........................'

    source = vtkHyperTreeGridSource()
    source.SetMaxDepth(6)
    source.SetDimensions(4, 4, 3)  # GridCell 3, 3, 2
    source.SetGridScale(1.5, 1.0, 0.7)
    source.SetBranchFactor(4)
    source.SetDescriptor(descriptor)
    source.Update()
    writeHTDS("test_htg.vtk", source.GetOutput())

    # Hyper tree grid to unstructured grid filter
    #htg2ug = vtkHyperTreeGridToUnstructuredGrid()
    #htg2ug.SetInputConnection(source.GetOutputPort())
    #htg2ug.Update()

    #shrink = vtkShrinkFilter()
    #shrink.SetInputConnection(htg2ug.GetOutputPort())
    #shrink.SetShrinkFactor(.8)
    #shrink.Update()
    #writeUSDS("test_unstru_grid.vtk", shrink.GetOutput())
    


if __name__ == '__main__':
    main()