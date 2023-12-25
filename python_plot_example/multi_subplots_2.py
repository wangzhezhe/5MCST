
import sys
import random
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib import ticker
import statistics
from matplotlib.patches import Patch
from matplotlib.lines import Line2D

labelSize = 16
tickSize = 16
legendsize=21
simSycle=0

figsize_x = 5.5
figsize_y = 3

def draw_figure(ax,data_name,figid):
    # colecting data
    # we use random values here
    xpos = range(0, 100,1)
    time_list=[]
    for i in range(100):
        time_list.append(random.randint(1,10))
    
    ax.bar(xpos,time_list)

    ax.set_xticks([])
    if data_name=="Tokamak":
        ax.set_ylabel('Y labes', fontsize=labelSize)
        ax.tick_params(axis='y', labelsize=tickSize)
    else:
        ax.set_yticks([])
    ax.title.set_text(figid)
    ax.title.set_fontsize(labelSize)

if __name__ == "__main__":
    # specify datasets name
    figure_names = ["Tokamak","Supernova","Hydraulics","CloverLeaf3D","Synthetic"]
   
    # specify num of col and rows for subfigures
    nr=5
    nc=5
    fig, axs = plt.subplots(nrows=nr, ncols=nc, figsize=(figsize_x*5,figsize_y*nr)) 
    # setting spaces between subfigures
    plt.subplots_adjust(wspace=0.1, hspace=0.2)
    
    for rindex in range(nr):
        for cindex in range(nc):
            figid = "("+str(cindex+1)+")"
            draw_figure(axs[rindex][cindex],figure_names[cindex],figid)

    fig.text(0.5, 0.08, 'X labels', ha='center',fontsize=labelSize)

    fig.savefig("test_multiple_subfigures.png",bbox_inches='tight',dpi=200)
    fig.savefig("test_multiple_subfigures.pdf",bbox_inches='tight')



