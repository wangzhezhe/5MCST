



import numpy as np


for i in range(0,50,1):
    action = np.random.choice(np.array([0,1]), p=[0.1,0.9])
    print(action)