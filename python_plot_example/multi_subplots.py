
import matplotlib.pyplot as plt
from matplotlib.patches import Patch

import numpy as np
import random


if __name__ == "__main__":
    num_plots = 5
    fig, axs = plt.subplots(num_plots, 1)
    #fig, axs = plt.subplots(num_plots, 1 figsize=(figsize_x,ffigsize_y))
    print(len(axs)) # there are three plot
    # we can set x and y separately for each subplot
    for plot_id in range(0,num_plots,1):
        # generating x and y
        x_list=range(0,10)
        random_y_list = []
        for i in range(0,10):
            n = random.randint(1,10)
            random_y_list.append(n)

        axs[plot_id].bar(x_list,random_y_list)

        # set the x lim and x ticks
        axs[plot_id].set_xlim([0, 12])
        if plot_id == num_plots-1:
            #set the ticks as needed according to the figure size
            #axs[plot_id].set_xticks((0,figsize_x/2,figsize_x))
            #axs[plot_id].set_xticklabels((0, 6 , 12))
            axs[plot_id].set_xlabel('x_label')        
        else:
            # only set the x tick for the bottom one
            axs[plot_id].tick_params(labelbottom=False)

        # set y labels and y ticks
        axs[plot_id].set_ylabel("y_label"+str(plot_id))
        # set y limitation
        axs[plot_id].set_ylim(0, 10)
        
        # remove the tick at the left side
        axs[plot_id].tick_params(labelleft=False)
        # set tick labels as empty
        axs[plot_id].set_yticklabels([])
        #remove that small tick line
        axs[plot_id].yaxis.set_ticks_position('none')


    fig.savefig('multi_subplots.png')



