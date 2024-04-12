import sys
sys.path.append('/ccs/home/zw241/cworkspace/install_adios2/lib/python3.10/site-packages')
import adios2
from adios2 import Stream
import numpy as np
filename = 'output_vki.bp'
vx = list()
vy = list()
vz = list()
cx = list()
cy = list()
cz = list()
pressure = list()
rho = list()
conn = list()

with adios2.Stream(filename, "r") as s:
    #print(fh)
    for _ in s.steps():
        for bid in range(2):
            #for name, info in s.available_variables().items():
            #    print("variable_name: " + name, end=" ")
            #    for key, value in info.items():
            #        print("\t" + key + ": " + value, end=" ")
            #    print()
            print("bid is",bid)
            #fh.read('/hpMusic_base/hpMusic_Zone/FlowSolution/RHE')
            print(len(s.read('FlowSolution/RHE',block_id=bid)))
            #vx.append(fh.read('/hpMusic_base/hpMusic_Zone/FlowSolution/RVX', block_id=bid))
            #vy.append(fh.read('/hpMusic_base/hpMusic_Zone/FlowSolution/RVY', block_id=bid))
            #vz.append(fh.read('/hpMusic_base/hpMusic_Zone/FlowSolution/RVZ', block_id=bid))
            #cx.append(fh.read('/hpMusic_base/hpMusic_Zone/GridCoordinates/CoordinateX', block_id=bid))
            #cy.append(fh.read('/hpMusic_base/hpMusic_Zone/GridCoordinates/CoordinateY', block_id=bid))
            #cz.append(fh.read('/hpMusic_base/hpMusic_Zone/GridCoordinates/CoordinateZ', block_id=bid))
            #pressure.append(fh.read('/hpMusic_base/hpMusic_Zone/FlowSolution/RHE', block_id=bid))
            #rho.append(fh.read('/hpMusic_base/hpMusic_Zone/FlowSolution/RHO', block_id=bid))
            #conn.append(fh.read('/hpMusic_base/hpMusic_Zone/Elem/ElementConnectivity', block_id=bid))

print(conn)