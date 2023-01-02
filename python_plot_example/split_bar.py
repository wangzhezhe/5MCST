import matplotlib.pyplot as plt
from matplotlib.patches import Patch

import numpy as np

gblue = '#4486F4'
gred = '#DA483B'
gyellow = '#FFC718'
ggreen = '#1CA45C'

if __name__ == "__main__":
	# split into two parts for one bar
    fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True,figsize=(7,4.6))
    # set titles
    fig.text(0.05, 0.5, 'Time(s)', va='center', rotation='vertical', fontsize=12)
    ax1.set_ylim([60, 100])
    ax2.set_ylim([0, 30])
    ax2.set_xlabel('The percentage of the qualified data', fontsize='large')
    # delet the line at the bottom and the top of the two figures
    ax1.spines['bottom'].set_visible(False)
    ax2.spines['top'].set_visible(False)
    ax1.tick_params(axis='x', bottom=False, labeltop=False, labelbottom=False)  # don't put tick labels at the top
    ax2.xaxis.tick_bottom()

    d = 0.01  # how big to make the diagonal lines in axes coordinates
    # arguments to pass to plot, just so we don't keep repeating them
    kwargs = dict(transform=ax1.transAxes, color='k', clip_on=False)
    ax1.plot((-d, +d), (-d, +d), **kwargs)  # top-left diagonal
    ax1.plot((1 - d, 1 + d), (-d, +d), **kwargs)  # top-right diagonal

    kwargs.update(transform=ax2.transAxes)  # switch to the bottom axes
    ax2.plot((-d, +d), (1 - d, 1 + d), **kwargs)  # bottom-left diagonal
    ax2.plot((1 - d, 1 + d), (1 - d, 1 + d), **kwargs)  # bottom-right diagonal

    plt.subplots_adjust(hspace=.1)  # adjust distance between two subplots

    ax1.set_axisbelow(True)
    ax2.set_axisbelow(True)

    # set tick
    N = 6
    ind = np.arange(N)    # the x locations for the groups
    width = 0.18       # the width of the bars
    ax1.set_xticks(ind + 1.5*width)
    ax1.set_xticklabels(('0%','20%','40%','60%','80%','100%'),fontsize='large')

    # set the value of the figure here
    # use the capsize to control the error bar
    tpMeans = (65.316,   67.43356667, 70.38986667, 72.1859, 74.23376667, 76.35183333 )
    tpStd = (0.6099340374,   0.04839900137,   0.5647084056 ,   0.5284408482,    0.8171366246 ,   2.759014564 )
    p1 = ax1.bar(ind, tpMeans,  width, color=[gblue]*5,yerr=tpStd, ecolor='black', capsize=3)

    tcMeans = (70.86473333,  71.28623333, 71.21433333, 71.20403333, 71.39376667, 71.64516667 )
    tcStd = (0.2115885236,   0.03775398434 ,  0.39333012,  0.8282423941,    0.560990502, 0.8876905392 )
    p2 = ax1.bar(ind + width, tcMeans,  width, color=[gred]*5, yerr=tcStd, ecolor='black', capsize=3)


    mpMeans = (78.61916667,  80.78726667, 81.15106667, 78.2954, 80.83366667, 81.21966667)
    mpStd = (1.255949467,    1.611722049, 3.626644518, 1.956503286, 2.674718677, 3.701866909)
    p3 = ax1.bar(ind + 2*width, mpMeans,  width, color=[gyellow]*5, yerr=mpStd, ecolor='black', capsize=3)

    tmMeans = (83.0089,  80.84883333, 81.65736667, 81.65436667, 81.4867, 82.08586667)
    tmStd = (1.304215316 ,   0.3882281589 ,   0.6588356725 ,   0.7596826991 ,   0.8035335774 ,   2.129914957)
    p4 = ax1.bar(ind + 3*width, tmMeans,  width, color=[ggreen]*5, yerr=tmStd, ecolor='black', capsize=3)
    

    p1 = ax2.bar(ind, tpMeans,  width, color=[gblue]*6,yerr=tpStd, ecolor='black', capsize=3)
    p2 = ax2.bar(ind + width, tcMeans,  width, color=[gred]*6, yerr=tcStd, ecolor='black', capsize=3)
    p3 = ax2.bar(ind + 2*width, mpMeans,  width, color=[gyellow]*6, yerr=mpStd, ecolor='black', capsize=3)
    p4 = ax2.bar(ind + 3*width, tmMeans,  width, color=[ggreen]*6, yerr=tmStd, ecolor='black', capsize=3)


    # add the lengend for the data by defualt
    # this can be at the centric legend
    ax1.legend((p1[0], p2[0], p3[0], p4[0]), ('producer-responsible', 'consumer-responsible','metadata-responsible','notification-responsible'),bbox_to_anchor=(0.83, 1.0), ncol=2, fontsize='medium')
    #ax1.legend((p1[0], p2[0], p3[0], p4[0]), ('producer-responsible', 'consumer-responsible','metadata-responsible','notification-responsible'),fontsize='large')
    # customize the legend
    #legend_elem_1 = [Patch(facecolor='#B4E1FF', edgecolor='black', label='Cache hit at remote DTN'),
    #                     Patch(facecolor='#AB87FF', edgecolor='black', label='Cache hit at local DTN')]
    #legend1 = plt.legend(handles=legend_elem_1, loc='upper center', ncol=2, bbox_to_anchor=(0.475, 1.1), fontsize=12)
    #ax.add_artist(legend1)
    #plt.savefig("exp_512_variantpercentage_sim_greater_ana.pdf",bbox_inches='tight')
    plt.savefig("exp_512_variantpercentage_sim_greater_ana.png",bbox_inches='tight')