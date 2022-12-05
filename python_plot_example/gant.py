import numpy as np
import matplotlib
import matplotlib.pyplot as plt

if __name__ == "__main__":
    fig, ax= plt.subplots()
    xlist=[(110, 30), (150, 10)]
    ax.set_xlabel('xlabesl')
    ax.broken_barh(xlist,yrange=(10,9))
    fig.savefig("gant.png",bbox_inches='tight')