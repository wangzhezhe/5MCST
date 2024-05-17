
This data is widely used in all kinds of paper:
https://www.earthsystemgrid.org/dataset/isabeldata.html

Undecompressed data can be load by paraview direacly throug image reader
with properly seting the bounds:

Format
The data is in "Brick-of-Floats" format. It consists of a volume of data values at each position in space. The three-dimensional array of data consists of planes of x-y values in ascending z order; in the data, the x values vary fastest. Assuming the data was stored as a one-dimensional array, the index into that array for the point x, y, z would be:

index=x+dim_x×(y+dim_y×z)
where dim_x is the number of x values (500) and dim_y is the number of y values (500).