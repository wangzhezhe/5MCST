import math

# transfer a set of points of airfoil to gmsh file

fo=open("a18.dat", "r")
line_num=0
point_list=[]
for line in fo:
	if line_num==0:
		line_num+=1
		continue
	line_strip=line.strip()
	line_split=line_strip.split('   ')
	point_list.append([line_split[0],line_split[1]])
	line_num+=1
fo.close()

#print(point_list)

# create the geo file based on point_list
# format
# Point(1) = {0.4, 2.1, 0, 1.0};
# Point(2) = {0.9, 2.6, 0, 1.0};
# Line(1) = {1, 2};
f = open("a18_airfoil_large_rotation.geo",'w')
# do sth to create the str for writting

point_index=1
# clockwise
angle = -15
# rotation along this point
rpx=0.5
rpy=0.125
for p in point_list:
	f.write("//+\n")
	px=float(p[0])
	py=float(p[1])
	px_new = (px-rpx)*math.cos(angle*math.pi/180.0)-(py-rpy)*math.sin(angle*math.pi/180.0)+rpx
	py_new = (py-rpy)*math.cos(angle*math.pi/180.0)+(px-rpx)*math.sin(angle*math.pi/180.0)+rpy
	
	point_str = "Point(%d) = {%s, %s, 0, 1.0};" % (point_index,px_new,py_new)
	f.write(point_str+"\n")
	point_index+=1
	
# write length
f.write("\n")
line_index=1
for pindex in range(1,point_index-1,1):
	f.write("//+\n")
	line_str = "Line(%d) = {%d, %d};"%(line_index,pindex,pindex+1)
	f.write(line_str+"\n")
	line_index+=1

# remember to close the loop !!! 
f.write("//+\n")
line_str = "Line(%d) = {%d, %d};"%(line_index,point_index-1,1)
f.write(line_str+"\n")
line_index+=1

largest_airfoil_index=line_index
# add bounds

bounds_list=[[-1,-1], [-1,1.25], [5.0,1.25], [5.0,-1]]
f.write("\n")
for p in bounds_list:
	f.write("//+\n")
	point_str = "Point(%d) = {%s, %s, 0, 1.0};" % (point_index,p[0],p[1])
	f.write(point_str+"\n")
	point_index+=1

f.write("//+\n")
line_str = "Line(%d) = {%d, %d};"% (line_index,point_index-4,point_index-3)
f.write(line_str+"\n")
line_index+=1

f.write("//+\n")
line_str = "Line(%d) = {%d, %d};"% (line_index,point_index-3,point_index-2)
f.write(line_str+"\n")
line_index+=1

f.write("//+\n")
line_str = "Line(%d) = {%d, %d};"% (line_index,point_index-2,point_index-1)
f.write(line_str+"\n")
line_index+=1

f.write("//+\n")
line_str = "Line(%d) = {%d, %d};"% (line_index,point_index-1,point_index-4)
f.write(line_str+"\n")
line_index+=1


# add curve loop based on line
curve_loop_1 = "Curve Loop(1) = {%d, %d, %d, %d};" % (line_index-1,line_index-2,line_index-3,line_index-4)
curve_loop_2 = "Curve Loop(2) = {"
for line_index in range(1,largest_airfoil_index,1):
	if line_index == largest_airfoil_index-1:
		curve_loop_2=curve_loop_2+str(line_index)
	else:
		curve_loop_2=curve_loop_2+str(line_index)+","
		
curve_loop_2+="};"
f.write("//+\n")
f.write(curve_loop_1+"\n")
f.write("//+\n")
f.write(curve_loop_2+"\n")
f.write("//+\n")
f.write("Surface(1) = {1, 2};"+"\n")

# reset mesh size of points
mesh_size = "MeshSize {"
for pi in range(1,point_index,1):
	if pi == point_index-1:
		mesh_size=mesh_size+str(pi)
	else:
		mesh_size=mesh_size+str(pi)+","
mesh_size+="} = 0.03;"
f.write("//+\n")
f.write(mesh_size+"\n")

# add volume

volume_str = "Physical Volume(\"internal\") = {1}; \
\nExtrude {0, 0, 0.1} {\
\nSurface{1};\
\nLayers{1};\
\nRecombine;\
\n}"
f.write("//+\n")
f.write(volume_str+"\n")


# physical group

physical_group_str="Physical Surface(\"inlet\", 273) = {99}; \
\nPhysical Surface(\"outlet\", 274) = {107}; \
\nPhysical Surface(\"frontandback\", 275) = {272, 1};\
\nPhysical Surface(\"top\", 276) = {103}; \
\nPhysical Surface(\"bottom\", 277) = {95}; \
\n//Printf(\"Debug surfaces\", Surface {:}); \
\nPhysical Surface(\"wall\", 278) = {111,115,119,123,127,131,135,139,143,147,151,155,159,163,167,171,175,179,183,187,191,195,199,203,207,211,215,219,223,227,231,235,239,243,247,251,255,259,263,267,271};"
f.write(physical_group_str+"\n")

f.close() 

