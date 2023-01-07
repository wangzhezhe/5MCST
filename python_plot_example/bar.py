import matplotlib.pyplot as plt
from matplotlib.patches import Patch

import numpy as np

gblue = '#4486F4'
gred = '#DA483B'
gyellow = '#FFC718'
ggreen = '#1CA45C'

if __name__ == "__main__":
    # typical bar example
    fig, ax = plt.subplots(figsize=(7,4.6))

    # set titles
    
    ax.set_xlabel('Data size per step', fontsize='large')
    ax.set_ylabel('Time(ms)', fontsize='large')

    # set tick
    N = 5
    ind = np.arange(N)    # the x locations for the groups
    width = 0.25       # the width of the bars
    ax.set_xticks(ind + width)
    ax.set_xticklabels(('32MB','64MB','128MB','256MB','512MB'), fontsize='large')

    # set the value of the figure here
    # use the capsize to control the error bar
    simMeans = (573.421,1098.675,1098.67,1649.21,2977.24)
    simStd = (0.1,0.1,0.1,0.1,0.1)
    p1 = ax.bar(ind, simMeans,  width, color=[gblue]*5, capsize=3, yerr=simStd, )

    checkMeans = (20.053,72.026,72.026,  124.274, 285.972)
    checkStd = ()
    p2 = ax.bar(ind + width, checkMeans,  width, color=[gred]*5, capsize=3)

    anaMeans = (1630.73, 2596.32, 2596.328,    3278.57, 4167.81)
    anaStd = ()
    p3 = ax.bar(ind + 2*width, anaMeans,  width, color=[gyellow]*5, ecolor='black', capsize=3)

    # add the lengend for the data by defualt
    #ax.legend((p1[0], p2[0], p3[0]), ('data generating', 'data checking','data analytics'), fontsize='large')
    
    # customize the legend
    #legend_elem_1 = [Patch(facecolor='#B4E1FF', edgecolor='black', label='label1'),
    #                     Patch(facecolor='#AB87FF', edgecolor='black', label='label2')]
    #legend1 = plt.legend(handles=legend_elem_1, loc='upper center', ncol=2, bbox_to_anchor=(0.475, 1.0), fontsize=12)
    #ax.add_artist(legend1)
    
    plt.savefig("exp_componentTime_sim_less_ana.png",bbox_inches='tight')
    #plt.savefig("exp_componentTime_sim_less_ana.pdf",bbox_inches='tight')