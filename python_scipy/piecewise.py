

import numpy as np
import matplotlib.pyplot as plt
import pwlf
import math
# generate data sets

xlist=np.arange(0, 3, 0.1).tolist()
print(xlist)
ylist=[]
# generating some test data
for x in xlist:
    if x<1:
        y=-0.2*x+1.2
        ylist.append(y)
    elif x>=1 and x <1.5:
        y=-x+1.5
        ylist.append(y)
    elif x>=1.5:
        y=-0.1*x+0.25
        ylist.append(y)

fig, ax = plt.subplots()
ax.plot(xlist,ylist)
plt.savefig("piecewise_input.png", bbox_inches='tight')


# using the piecewise linear regression
# initialize piecewise linear fit with your x and y data
my_pwlf = pwlf.PiecewiseLinFit(xlist, ylist)

# fit the data for four line segments
res = my_pwlf.fit(4)

# predict for the determined points
xHat = np.linspace(min(xlist), max(xlist), num=10000)
yHat = my_pwlf.predict(xHat)
print(my_pwlf)

plt.clf()
plt.figure()
plt.plot(xlist, ylist, 'o')
plt.plot(xHat, yHat, '-')
plt.savefig("piecewise_test.png", bbox_inches='tight')

for id,x in enumerate(xHat):
    if id==0:
        continue
    if id==len(xHat)-1:
        continue
    if math.fabs((yHat[id-1]-yHat[id]) - (yHat[id]-yHat[id+1])) > 0.000001:
        print(x)

# this is a example to find the best line of segment
        
