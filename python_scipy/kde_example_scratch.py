# The kde approach from scratch
# Input is the original observations
# Output is the 
#(1) kde, which is the approximation to the 
#(2) kde, which has the density representation

# code come from here, a really good blog
# https://billc.io/2023/01/kde-from-scratch/
# This is another good tutorial
# https://bookdown.org/egarpor/NP-UC3M/kde-ii-mult.html
from matplotlib import pyplot
from numpy.random import normal
import numpy as np
import scipy.stats as stats
from scipy.stats import norm
from numpy import hstack
from numpy import asarray
from numpy import exp
from sklearn.neighbors import KernelDensity

# there kernel are standard unit
def kernel(k: str):
    """Kernel Functions.
    Ref: https://en.wikipedia.org/wiki/Kernel_(statistics)

    Args:
        k (str): Kernel name. Can be one of ['gaussian', 'epanechnikov', 'cosine', 'linear'.]
    """

    if k not in ['gaussian', 'epanechnikov', 'cosine', 'linear']:
        raise ValueError('Unknown kernel.')

    def bounded(f):
        def _f(x):
            return f(x) if np.abs(x) <= 1 else 0
        return _f

    if k == 'gaussian':
        return lambda u: 1 / np.sqrt(2 * np.pi) * np.exp(-1 / 2 * u * u)
    elif k == 'epanechnikov':
        return bounded(lambda u: (3 / 4 * (1 - u * u)))
    elif k =='cosine':
        return bounded(lambda u: np.pi / 4 * np.cos(np.pi / 2 * u))
    elif k == 'linear':
        return bounded(lambda u: 1 - np.abs(u))

# three ways to find the suitable h
def bw_scott(data: np.ndarray):
    std_dev = np.std(data, axis=0, ddof=1)
    print("std_dev",std_dev)
    n = len(data)
    return 3.49 * std_dev * n ** (-0.333)

def bw_silverman(data: np.ndarray):
    def _select_sigma(x):
        normalizer = 1.349
        iqr = (stats.scoreatpercentile(x, 75) - stats.scoreatpercentile(x, 25)) / normalizer
        std_dev = np.std(x, axis=0, ddof=1)
        return np.minimum(std_dev, iqr) if iqr > 0 else std_dev
    sigma = _select_sigma(data)
    n = len(data)
    return 0.9 * sigma * n ** (-0.2)

def bw_mlcv(data: np.ndarray, k):
    """
    Ref: https://rdrr.io/cran/kedd/src/R/MLCV.R
    """
    n = len(data)
    x = np.linspace(np.min(data), np.max(data), n)
    def mlcv(h):
        fj = np.zeros(n)
        for j in range(n):
            for i in range(n):
                if i == j: continue
                fj[j] += k((x[j] - data[i]) / h)
            fj[j] /= (n - 1) * h
        return -np.mean(np.log(fj[fj > 0]))
    h = optimize.minimize(mlcv, 1)
    if np.abs(h.x[0]) > 10:
        return bw_scott(data)
    return h.x[0]

def kde(data, k=None, h=None, x=None):
    """Kernel Density Estimation.

    Args:
        data (np.ndarray): Data.
        k (function): Kernel function.
        h (float): Bandwidth.
        x (np.ndarray, optional): Grid. Defaults to None.

    Returns:
        np.ndarray: Kernel density estimation.
    """
    # line space is between the min and max of the input data
    # it means how many samples at x axis
    if x is None:
        x = np.linspace(np.min(data), np.max(data), 1000)
    if h is None:
        h = bw_silverman(data)
    if k is None:
        k = kernel('gaussian')
    n = len(data)
    kde = np.zeros_like(x)
    # using the kde function to accumulate the positions
    # at each obvervation points
    for j in range(len(x)):
        for i in range(n):
            kde[j] += k((x[j] - data[i]) / h)
        kde[j] /= n * h
    return kde


# generating sample for testing
sample1 = normal(loc=20, scale=5, size=300)
sample2 = normal(loc=40, scale=5, size=700)
sample = hstack((sample1, sample2))

#print(sample)

# get the range of the data
x_points = np.linspace(np.min(sample), np.max(sample), 1000)
#pyplot.hist(sample, bins=50, density=True)

# pay attention, this kde_result is actually the histogram
# the input is the original data
# the x is the sampled data we want to extract from the histogram 
kde_result=kde(sample)
# this is the histogram results
#pyplot.plot(x_points,kde_result)
#pyplot.savefig('kde_sample_results.png')


# test the bw_scott 
test_x_points = np.linspace(1, 10, 10)
print(test_x_points)
print("bw_scott", bw_scott(test_x_points))


test_x_points_2 = np.linspace(0, 1, 3)
print(test_x_points_2)
k = kernel('gaussian')
print('gaussian', k(test_x_points_2))

# the input value for x
sampleTestList=[23.40953488,20.06452322,14.63870771,21.61275585,29.82232875,11.23601927]
targetedX=np.array([10.0,15.0,20.0])
#targetedX=np.linspace(np.min(sampleTestList), np.max(sampleTestList), 1000)
sampleTestKde=kde(sampleTestList,k=kernel('gaussian'), h=bw_scott(sampleTestList), x=targetedX)
print(sampleTestKde)


#draw figure to show results
print("test bw_scott for new data", bw_scott(sampleTestList))
x_points = np.linspace(np.min(sampleTestList), np.max(sampleTestList), 100)
kde_result=kde(sampleTestList,k=kernel('gaussian'), h=bw_scott(sampleTestList), x=x_points)
pyplot.plot(x_points,kde_result)
pyplot.savefig('kde_sample_results_temp.png')