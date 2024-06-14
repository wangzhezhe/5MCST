Data source of redsea can be found here:

https://kaust-vislab.github.io/SciVis2020/

The original data is the unstructured rectlinear grid

There is a convenient python script based on paraview to do that

Be careful about the transform filter, it reset the scale of the original dataset (resetting the space of the data set)

This extract.py is the version that can work on my laptop

remember to download both bathymetry.xml and bathymetry.h5