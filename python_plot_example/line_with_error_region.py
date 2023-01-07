# refer to this
# https://hernandis.me/2020/04/05/three-examples-of-nonlinear-least-squares-fitting-in-python-with-scipy.html



import matplotlib.pyplot as plt
from matplotlib.patches import Patch

import numpy as np

gblue = '#4486F4'
gred = '#DA483B'
gyellow = '#FFC718'
ggreen = '#1CA45C'



if __name__ == "__main__":
    fig, ax = plt.subplots(figsize=(9,4.5))
    # set titles
    ax.set_xlabel('x_label', fontsize='large')
    ax.set_ylabel('y_label', fontsize='large')
    ax.grid(axis='y')
    ax.set_axisbelow(True)

    ax.set_ylim([0,2.5])

    # set tick
    N = 5

    # set the value of the figure here
    # use the capsize to control the error bar
    x_size = 9
    unit = x_size/N*1.0
    temp_list = list(range(1,N+1))
    x_list = [item * unit for item in temp_list]
    plt.xticks(x_list, ['tick1', 'tick2' , 'tick3', 'tick4', 'tick5'], fontsize='large')

    tpMeans = (0.71329225,  0.84356025,  0.917 ,  1.00394475 , 1.02399525)
    p1 = ax.plot(x_list, tpMeans, color=gblue, marker='^', label='ds_put')
    
    # set the fill region
    y_low_limit=[]
    y_upper_limit=[]
    for i,v in enumerate(tpMeans):
        y_low_limit.append(v-0.1)
        y_upper_limit.append(v+0.1)
    
    ax.fill_between(x_list, y_low_limit, y_upper_limit, alpha = 0.2, color = 'green')



    # add the lengend for the data by defualt
    # this can be at the centric legend
    ax.legend(ncol=2, fontsize='large')
    plt.savefig("exp_weakscale.png", bbox_inches='tight')