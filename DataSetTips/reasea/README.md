Data source of redsea can be found here:

https://kaust-vislab.github.io/SciVis2020/

The original data is the unstructured rectlinear grid

remember to download both bathymetry.xml and bathymetry.h5

There is a convenient python script based on paraview to do that

Be careful about the transform filter, it reset the scale of the original dataset (resetting the space of the data set)

This extract.py is the version that can work on my laptop

We can save the raw data into vtk file direactly

If we want the velocity magnitude, we just need to use the caculator filter to create a new expression, let `velocityMagnitude=sqrt(U*U+V*V+W*W)`

Since the velocity magnitude is computed by ParaView automatically

another way to save the data is to open the spread sheet and then loading the CSV file through python.

Then using the decompose.py to extract the ensmeble members by each layer.

Each layer is an ensemble element in this situation.


Extracting data through paraview (although it is ok to use pvbatch, executing these code through paraview direactly can avoid some depedency error)

```
#load data
file_name='redsea_raw_ens10.vtk'
file_path='/Users/zhewang/DataSets/redsea_raw/redsea_raw_ens10.vtk'
save_data_file='/Users/zhewang/DataSets/redsea_raw/redsea_raw_ens10_layer49.vtk'

redsea_raw_vtk = LegacyVTKReader(registrationName=file_name, FileNames=[file_path])

extractSubset = ExtractSubset(Input=redsea_raw_vtk,VOI = [0, 499, 0, 499, 49, 49])

# save data
SaveData(save_data_file, proxy=extractSubset, ChooseArraysToWrite=1,
    PointDataArrays=['SALT', 'TEMP', 'UVW', 'velocityMagnitude'])
```