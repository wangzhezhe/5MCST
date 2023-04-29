import vtk

def writeHTDS(fname, ds) :
    writer = vtk.vtkXMLHyperTreeGridWriter()
    writer.SetFileName(fname)
    writer.SetInputData(ds)
    writer.Update()
    writer.Write()


# several questions
# the data value is only associated with the cell?
# vtkmodules.vtkCommonDataModel.vtkHyperTreeGrid' object has no attribute 'GetPointData'
# refer to https://vtk.org/doc/nightly/html/classvtkHyperTree.html#details 
# (It seems that the value is onlu assocaited with cell data)
# and https://vtk.org/doc/nightly/html/classvtkHyperTreeGrid.html#details

# refer to this blog for more details
# https://www.kitware.com/hypertreegrid-in-vtk-data-construction/

# the data is associated with the cell
# this is different with the octLocator for grid data

# Declare htg
htg = vtk.vtkHyperTreeGrid()
htg.Initialize()

# Declare scalar
scalarArray = vtk.vtkDoubleArray()
scalarArray.SetName('MyScalar')
scalarArray.SetNumberOfValues(0)
htg.GetCellData().AddArray(scalarArray)
htg.GetCellData().SetActiveScalars('MyScalar')

# Construct the rectilinear grid
htg.SetDimensions([4, 3, 2])
htg.SetBranchFactor(2)

## X coordinates
xValues = vtk.vtkDoubleArray()
xValues.SetNumberOfValues(4)
xValues.SetValue(0, -1)
xValues.SetValue(1, 0)
xValues.SetValue(2, 1)
xValues.SetValue(3, 2)
htg.SetXCoordinates(xValues)

## Y coordinates
yValues = vtk.vtkDoubleArray()
yValues.SetNumberOfValues(3)
yValues.SetValue(0, -1)
yValues.SetValue(1, 0)
yValues.SetValue(2, 1)
htg.SetYCoordinates(yValues)

## Z coordinates
zValues = vtk.vtkDoubleArray()
zValues.SetNumberOfValues(2)
zValues.SetValue(0, 0)
zValues.SetValue(1, 1)
htg.SetZCoordinates(zValues)

# the HTG is empty at this step
# there are 6 number of trees in current step?
print("htg.GetMaxNumberOfTrees", htg.GetMaxNumberOfTrees())
print("htg.GetNumberOfLeaves",htg.GetNumberOfLeaves())
print("htg.GetNumberOfLevels",htg.GetNumberOfLevels())

# Declare cursor
cursor = vtk.vtkHyperTreeGridNonOrientedCursor()

# Initialise values offset
offsetIndex = 0

# Set cursor on cell #0 and set the start index
# the index here represent the index of original cell
htg.InitializeNonOrientedCursor(cursor, 0, True)
cursor.SetGlobalIndexStart(offsetIndex)  # cell 0

# Assigns an index for the value of the cell pointed to by the current cursor state 
idx = cursor.GetGlobalNodeIndex()
# one value is assigned to the acalar array and binded with assocaited tree
scalarArray.InsertTuple1(idx, 1)

# Decides to subdivide cell pointed by current cursor state
cursor.SubdivideLeaf()  # cell 0

# In this example, 4 child cells were constructed
# Move the cursor to the first child 
cursor.ToChild(0)  # cell 0/0
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 7)
cursor.ToParent()  # cell 0

# Move the cursor to the second child
cursor.ToChild(1)  # cell 0/1
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 8)
cursor.ToParent()  # cell 0

# And next
cursor.ToChild(2)  # cell 0/2
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 9)
cursor.ToParent()  # cell 0

# And next
cursor.ToChild(3)  # cell 0/3
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 10)
cursor.ToParent()  # cell 0

# the index 0 is divided into for values
# and associated values are assigned to each sub cell

# Refine the cell 0
# this get number of vertices return the number of vertices under the current cursor
# for this case, there are 1 level 0 vertice (coarse vertice) and four leaf vertices
NumberOfVertices = cursor.GetTree().GetNumberOfVertices()
# there are 5 cell at the current step
offsetIndex += NumberOfVertices
print("offsetIndex",offsetIndex, "NumberOfVertices",NumberOfVertices)

# Set cursor on cell #3 and set the start index
htg.InitializeNonOrientedCursor(cursor, 1, True)
cursor.SetGlobalIndexStart(offsetIndex) # cell 1
idx = cursor.GetGlobalNodeIndex()
print ("GetGlobalNodeIndex for cell 1 is", idx)
scalarArray.InsertTuple1(idx, 2)


# Refined the cell 1
# there is only one vertice under the current cursor
NumberOfVertices2 = cursor.GetTree().GetNumberOfVertices()
offsetIndex += NumberOfVertices2
print("offsetIndex",offsetIndex, "NumberOfVertices2",NumberOfVertices2)

# Set cursor on cell #3 and set the start index
htg.InitializeNonOrientedCursor(cursor, 3, True)
cursor.SetGlobalIndexStart(offsetIndex) # cell 3
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 4)


# Refined the cell 3
offsetIndex += cursor.GetTree().GetNumberOfVertices()

# Set cursor on cell #5 and set the start index
htg.InitializeNonOrientedCursor(cursor, 5, True)
cursor.SetGlobalIndexStart(offsetIndex) # cell 5
idx = cursor.GetGlobalNodeIndex()
scalarArray.InsertTuple1(idx, 6)


# Refined the cell 5
offsetIndex += cursor.GetTree().GetNumberOfVertices()

# we do not adding more values here

writeHTDS("test_htg_3.vtk", htg)


'''
# the rendering pipeline
# Which ends the construction of my mesh
# Use a Geometry filter
geometry = vtk.vtkHyperTreeGridGeometry()
geometry.SetInputData(htg)

# Add, for example, a Shrink filter
shrink = vtk.vtkShrinkFilter()
shrink.SetInputConnection(geometry.GetOutputPort())
shrink.SetShrinkFactor(.8)


# LookupTable
lut = vtk.vtkLookupTable()
lut.SetHueRange(0.66, 0)
lut.UsingLogScale()
lut.Build()

# Mappers
mapper = vtk.vtkDataSetMapper()
mapper.SetInputConnection(shrink.GetOutputPort())
mapper.SetLookupTable(lut)
mapper.SetColorModeToMapScalars()
mapper.SetScalarModeToUseCellFieldData()
mapper.SelectColorArray('MyScalar')

# Actors
actor = vtk.vtkActor()
actor.SetMapper(mapper)

# Renderer
renderer = vtk.vtkRenderer()
renderer.ResetCamera()
renderer.GetActiveCamera().Azimuth(150)
renderer.GetActiveCamera().Elevation(30)
renderer.AddActor(actor)

# Render window
renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(renderer)
renWin.SetSize(600, 400)

# Render window interactor
iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)

# render the image
renWin.Render()
iren.Start()
'''