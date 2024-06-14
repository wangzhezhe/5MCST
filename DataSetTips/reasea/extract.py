# state file generated using paraview version 5.8.0


# ----------------------------------------------------------------
# setup views used in the visualization
# ----------------------------------------------------------------


# trace generated using paraview version 5.8.0
#
# To ensure correct image size when batch processing, please search
# for and uncomment the line `# renderView*.ViewSize = [*,*]`


#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()


# get the material library
materialLibrary1 = GetMaterialLibrary()


# Create a new 'Render View'
renderView1 = CreateView('RenderView')
renderView1.ViewSize = [2140, 1450]
renderView1.AxesGrid = 'GridAxes3DActor'
renderView1.OrientationAxesVisibility = 0
renderView1.CenterOfRotation = [40.00709915161133, 20.007100105285645, -0.39749999667401426]
renderView1.StereoType = 'Crystal Eyes'
renderView1.CameraPosition = [40.415371452696036, -8.177274962686637, 18.441584167502217]
renderView1.CameraFocalPoint = [39.999210031584724, 21.788767142983147, -3.699292435249955]
renderView1.CameraViewUp = [0.0064322426791889094, 0.5942987752578183, 0.8042186220059651]
renderView1.CameraFocalDisk = 1.0
renderView1.CameraParallelScale = 14.1194337619734
renderView1.BackEnd = 'OSPRay raycaster'
renderView1.OSPRayMaterialLibrary = materialLibrary1


SetActiveView(None)


# ----------------------------------------------------------------
# setup view layouts
# ----------------------------------------------------------------


# create new layout object 'Layout #1'
layout1 = CreateLayout(name='Layout #1')
layout1.AssignView(0, renderView1)


# ----------------------------------------------------------------
# restore active view
SetActiveView(renderView1)
# ----------------------------------------------------------------


# ----------------------------------------------------------------
# setup the data processing pipelines
# ----------------------------------------------------------------


# create a new 'NetCDF Reader'

netCDFReader1 = NetCDFReader(FileName=['/Users/zhewang/Downloads/redsea_raw/0010/COMBINED_2011013100.nc'])
netCDFReader1.Dimensions = '(Z_MIT40, YG, XC)'
netCDFReader1.SphericalCoordinates = 0


# create a new 'XDMF Reader'
xDMFReader1 = XDMFReader(FileNames=['/Users/zhewang/Downloads/redsea_raw/bathymetry.xmf'])
xDMFReader1.PointArrayStatus = ['depth']
xDMFReader1.GridStatus = ['Grid']


# create a new 'Warp By Scalar'
warpByScalar1 = WarpByScalar(Input=xDMFReader1)
warpByScalar1.Scalars = ['POINTS', 'depth']
warpByScalar1.ScaleFactor = 0.00025


# create a new 'NetCDF Reader'
netCDFReader3 = NetCDFReader(FileName=['/Users/zhewang/Downloads/redsea_raw/0010/COMBINED_2011013100.nc'])
netCDFReader3.Dimensions = '(Z_MIT40, YC, XG)'
netCDFReader3.SphericalCoordinates = 0


# create a new 'NetCDF Reader'
netCDFReader2 = NetCDFReader(FileName=['/Users/zhewang/Downloads/redsea_raw/0010/COMBINED_2011013100.nc'])
netCDFReader2.Dimensions = '(Z_MIT40, YC, XC)'
netCDFReader2.SphericalCoordinates = 0


# create a new 'Resample To Image'
resampleToImage3 = ResampleToImage(Input=netCDFReader2)
resampleToImage3.UseInputBounds = 0
resampleToImage3.SamplingDimensions = [500, 500, 50]
resampleToImage3.SamplingBounds = [30.027099609375, 49.96709869384765, 10.027099609375, 29.96710060119629, -3178.0, -2.0]


# create a new 'Resample To Image'
resampleToImage1 = ResampleToImage(Input=netCDFReader1)
resampleToImage1.UseInputBounds = 0
resampleToImage1.SamplingDimensions = [500, 500, 50]
resampleToImage1.SamplingBounds = [30.027099609375, 49.96709869384765, 10.027100105285646, 29.967100143432617, -3178.0, -2.0]


# create a new 'Resample To Image'
resampleToImage2 = ResampleToImage(Input=netCDFReader3)
resampleToImage2.UseInputBounds = 0
resampleToImage2.SamplingDimensions = [500, 500, 50]
resampleToImage2.SamplingBounds = [30.027099151611328, 49.96709823608399, 10.027099609375, 29.96710060119629, -3178.0, -2.0]


# create a new 'Append Attributes'
appendAttributes1 = AppendAttributes(Input=[resampleToImage1, resampleToImage2, resampleToImage3])


# create a new 'Calculator'
calculator1 = Calculator(Input=appendAttributes1)
calculator1.ResultArrayName = 'UVW'
calculator1.Function = 'U*iHat + V*jHat + W*kHat'


# create a new 'Transform'
transform1 = Transform(Input=calculator1)
transform1.Transform = 'Transform'


# init the 'Transform' selected for 'Transform'
transform1.Transform.Scale = [1.0, 1.0, 0.00025]


# create a new 'Glyph'
glyph1 = Glyph(Input=transform1,
   GlyphType='Arrow')
glyph1.OrientationArray = ['POINTS', 'UVW']
glyph1.ScaleArray = ['POINTS', 'UVW']
glyph1.ScaleFactor = 1.994000003814697
glyph1.GlyphTransform = 'Transform2'


# ----------------------------------------------------------------
# setup the visualization in view 'renderView1'
# ----------------------------------------------------------------


# show data from transform1
transform1Display = Show(transform1, renderView1, 'StructuredGridRepresentation')


# trace defaults for the display properties.
transform1Display.Representation = 'Outline'
transform1Display.ColorArrayName = ['POINTS', '']
transform1Display.OSPRayScaleArray = 'SALT'
transform1Display.OSPRayScaleFunction = 'PiecewiseFunction'
transform1Display.SelectOrientationVectors = 'UVW'
transform1Display.ScaleFactor = 1.996000099182129
transform1Display.SelectScaleArray = 'None'
transform1Display.GlyphType = 'Arrow'
transform1Display.GlyphTableIndexArray = 'None'
transform1Display.GaussianRadius = 0.09980000495910644
transform1Display.SetScaleArray = ['POINTS', 'SALT']
transform1Display.ScaleTransferFunction = 'PiecewiseFunction'
transform1Display.OpacityArray = ['POINTS', 'SALT']
transform1Display.OpacityTransferFunction = 'PiecewiseFunction'
transform1Display.DataAxesGrid = 'GridAxesRepresentation'
transform1Display.PolarAxes = 'PolarAxesRepresentation'
transform1Display.ScalarOpacityUnitDistance = 0.12266336815460342
transform1Display.SelectInputVectors = ['POINTS', 'UVW']
transform1Display.WriteLog = ''


# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
transform1Display.ScaleTransferFunction.Points = [0.0, 0.0, 0.5, 0.0, 42.474082946777344, 1.0, 0.5, 0.0]


# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
transform1Display.OpacityTransferFunction.Points = [0.0, 0.0, 0.5, 0.0, 42.474082946777344, 1.0, 0.5, 0.0]


# show data from glyph1
glyph1Display = Show(glyph1, renderView1, 'GeometryRepresentation')


# get color transfer function/color map for 'TEMP'
tEMPLUT = GetColorTransferFunction('TEMP')
tEMPLUT.RGBPoints = [0.0, 0.231373, 0.298039, 0.752941, 15.839231491088867, 0.865003, 0.865003, 0.865003, 31.678462982177734, 0.705882, 0.0156863, 0.14902]
tEMPLUT.ScalarRangeInitialized = 1.0


# trace defaults for the display properties.
glyph1Display.Representation = 'Surface'
glyph1Display.ColorArrayName = ['POINTS', 'TEMP']
glyph1Display.LookupTable = tEMPLUT
glyph1Display.OSPRayScaleArray = 'SALT'
glyph1Display.OSPRayScaleFunction = 'PiecewiseFunction'
glyph1Display.SelectOrientationVectors = 'None'
glyph1Display.ScaleFactor = 2.0431316375732425
glyph1Display.SelectScaleArray = 'None'
glyph1Display.GlyphType = 'Arrow'
glyph1Display.GlyphTableIndexArray = 'None'
glyph1Display.GaussianRadius = 0.10215658187866211
glyph1Display.SetScaleArray = ['POINTS', 'SALT']
glyph1Display.ScaleTransferFunction = 'PiecewiseFunction'
glyph1Display.OpacityArray = ['POINTS', 'SALT']
glyph1Display.OpacityTransferFunction = 'PiecewiseFunction'
glyph1Display.DataAxesGrid = 'GridAxesRepresentation'
glyph1Display.PolarAxes = 'PolarAxesRepresentation'
glyph1Display.SelectInputVectors = [None, '']
glyph1Display.WriteLog = ''


# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
glyph1Display.ScaleTransferFunction.Points = [0.0, 0.0, 0.5, 0.0, 40.65387778108867, 1.0, 0.5, 0.0]


# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
glyph1Display.OpacityTransferFunction.Points = [0.0, 0.0, 0.5, 0.0, 40.65387778108867, 1.0, 0.5, 0.0]


# show data from warpByScalar1
warpByScalar1Display = Show(warpByScalar1, renderView1, 'StructuredGridRepresentation')


# get color transfer function/color map for 'depth'
depthLUT = GetColorTransferFunction('depth')
depthLUT.RGBPoints = [-3073.121826171875, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0]
depthLUT.ColorSpace = 'RGB'
depthLUT.NanColor = [1.0, 0.0, 0.0]
depthLUT.ScalarRangeInitialized = 1.0


# get opacity transfer function/opacity map for 'depth'
depthPWF = GetOpacityTransferFunction('depth')
depthPWF.Points = [-3073.121826171875, 0.0, 0.5, 0.0, 0.0, 1.0, 0.5, 0.0]
depthPWF.ScalarRangeInitialized = 1


# trace defaults for the display properties.
warpByScalar1Display.Representation = 'Surface'
warpByScalar1Display.ColorArrayName = ['POINTS', 'depth']
warpByScalar1Display.LookupTable = depthLUT
warpByScalar1Display.OSPRayScaleArray = 'depth'
warpByScalar1Display.OSPRayScaleFunction = 'PiecewiseFunction'
warpByScalar1Display.SelectOrientationVectors = 'None'
warpByScalar1Display.ScaleFactor = 1.996000099182129
warpByScalar1Display.SelectScaleArray = 'depth'
warpByScalar1Display.GlyphType = 'Arrow'
warpByScalar1Display.GlyphTableIndexArray = 'depth'
warpByScalar1Display.GaussianRadius = 0.09980000495910644
warpByScalar1Display.SetScaleArray = ['POINTS', 'depth']
warpByScalar1Display.ScaleTransferFunction = 'PiecewiseFunction'
warpByScalar1Display.OpacityArray = ['POINTS', 'depth']
warpByScalar1Display.OpacityTransferFunction = 'PiecewiseFunction'
warpByScalar1Display.DataAxesGrid = 'GridAxesRepresentation'
warpByScalar1Display.PolarAxes = 'PolarAxesRepresentation'
warpByScalar1Display.ScalarOpacityFunction = depthPWF
warpByScalar1Display.ScalarOpacityUnitDistance = 0.44885145468837023
warpByScalar1Display.SelectInputVectors = [None, '']
warpByScalar1Display.WriteLog = ''


# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
warpByScalar1Display.ScaleTransferFunction.Points = [-3073.121826171875, 0.0, 0.5, 0.0, 0.0, 1.0, 0.5, 0.0]


# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
warpByScalar1Display.OpacityTransferFunction.Points = [-3073.121826171875, 0.0, 0.5, 0.0, 0.0, 1.0, 0.5, 0.0]


# setup the color legend parameters for each legend in this view


# get color legend/bar for tEMPLUT in view renderView1
tEMPLUTColorBar = GetScalarBar(tEMPLUT, renderView1)
# tEMPLUTColorBar.WindowLocation = 'AnyLocation'
tEMPLUTColorBar.Position = [0.8995967741935484, 0.6382978723404255]
tEMPLUTColorBar.Title = 'TEMP'
tEMPLUTColorBar.ComponentTitle = ''
tEMPLUTColorBar.ScalarBarLength = 0.32999999999999985


# set color bar visibility
tEMPLUTColorBar.Visibility = 1


# show color legend
glyph1Display.SetScalarBarVisibility(renderView1, True)


# ----------------------------------------------------------------
# setup color maps and opacity mapes used in the visualization
# note: the Get..() functions create a new object, if needed
# ----------------------------------------------------------------


# get opacity transfer function/opacity map for 'TEMP'
tEMPPWF = GetOpacityTransferFunction('TEMP')
tEMPPWF.Points = [0.0, 0.0, 0.5, 0.0, 31.678462982177734, 1.0, 0.5, 0.0]
tEMPPWF.ScalarRangeInitialized = 1


# ----------------------------------------------------------------
# finally, restore active source
SetActiveSource(warpByScalar1)
# ----------------------------------------------------------------

