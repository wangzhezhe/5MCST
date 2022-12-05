
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
    ax.set_xlabel('the number  of writers / the number of readers / the number of staging servers ', fontsize='large')
    ax.set_ylabel('Time(s)', fontsize='large')
    ax.grid(axis='y')
    ax.set_axisbelow(True)

    ax.set_ylim([0,2.5])

    # set tick
    N = 5
    #ind = np.arange(N)    # the x locations for the groups
    #width = 0.15       # the width of the bars
    #.set_xticks(ind + 1.5*width)
    #ax.set_xticklabels(('256/32/8', '512/64/16' , '1024/128/32', '2048/256/64', '4096/512/128'), fontsize='large')

    plt.xticks(range(N), ['256/32/8', '512/64/16' , '1024/128/32', '2048/256/64', '4096/512/128'], fontsize='large')
    # set the value of the figure here
    # use the capsize to control the error bar
    tpMeans = (0.71329225,  0.84356025,  0.917 ,  1.00394475 , 1.02399525)
    p1 = ax.plot(tpMeans, color=gblue, marker='^', label='ds_put')


    tpMeans = (1.059096 ,   1.26191, 1.638456 ,   2.06853 , 1.889604 )
    p2 = ax.plot( tpMeans, '--', color=gblue, marker='o', label='ds_get')

    tpMeans = (0.7725381667 ,   0.9162766667 ,   0.8464131667  ,  0.902399 ,   1.137003167)
    p3 = ax.plot(tpMeans, color=gred, marker='^', label='gorilla_put')


    tpMeans = (1.076468333 ,1.256378333 ,1.554481667, 1.94631 ,1.848243333 )
    p4 = ax.plot( tpMeans, '--', color=gred, marker='o', label='gorilla_get')
    

    # add the lengend for the data by defualt
    # this can be at the centric legend
    ax.legend(ncol=2, fontsize='large')
    # ax.legend((p1[0], p2[0], p3[0], p4[0]), ('producer-responsible', 'consumer-responsible','metadata-polling','topic-matching'),fontsize='large')
    # customize the legend
    #legend_elem_1 = [Patch(facecolor='#B4E1FF', edgecolor='black', label='Cache hit at remote DTN'),
    #                     Patch(facecolor='#AB87FF', edgecolor='black', label='Cache hit at local DTN')]
    #legend1 = plt.legend(handles=legend_elem_1, loc='upper center', ncol=2, bbox_to_anchor=(0.475, 1.1), fontsize=12)
    #ax.add_artist(legend1)
    plt.savefig("exp_weakscale.pdf", bbox_inches='tight')
    plt.savefig("exp_weakscale.png", bbox_inches='tight')