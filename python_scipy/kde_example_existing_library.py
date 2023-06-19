
# The source code comes from https://machinelearningmastery.com/probability-density-estimation/
# example of plotting a histogram of a random sample

from matplotlib import pyplot
from numpy.random import normal
import numpy as np
from scipy.stats import norm
from numpy import hstack
from numpy import asarray
from numpy import exp
from sklearn.neighbors import KernelDensity


# generate a sample
sample = normal(size=1000)
# plot a histogram of the sample
pyplot.hist(sample, bins=10)
#pyplot.show()
pyplot.savefig('normal_hist.png')

pyplot.clf()
# generate random sample of 1000 observation
# assuming we do not know the distribution

# Be careful, the input data here is the frequency data its self
# since we sample them from the normal distribution
# For actual data sets, what we get is the original histogram scale
# when we plot the results
# we need to transfer it to frequency scale manualy by dividing the toal number of observations
sample=normal(loc=50,scale=5,size=1000)
sample_mean=np.mean(sample)
sample_std=np.std(sample)
print("Mean =%.3f, stdev=%.3f" % (sample_mean,sample_std))

# define a distribution
dist = norm(sample_mean, sample_std)
# sample probabilities for a range of outcomes
values = [value for value in range(30, 70)]
probabilities = [dist.pdf(value) for value in values]
# plot the histogram and pdf
pyplot.hist(sample, bins=10, density=True)
pyplot.plot(values, probabilities)
#pyplot.show()
pyplot.savefig('normal_actual.png')

pyplot.clf()
# construct a bimodal distribution
# generate a sample
sample1 = normal(loc=20, scale=5, size=300)
sample2 = normal(loc=40, scale=5, size=700)
sample = hstack((sample1, sample2))
# plot the histogram
pyplot.hist(sample, bins=20)
pyplot.savefig('bimodal.png')

pyplot.clf()
# create a model and fit the observed data
model = KernelDensity(bandwidth=2, kernel='gaussian')
sample = sample.reshape((len(sample), 1))
model.fit(sample)

# sample probabilities for a range of outcomes
values = asarray([value for value in range(1, 60)])
#print(values)
values = values.reshape((len(values), 1))

#reshape value into one column
#print(values)
probabilities = model.score_samples(values)
# the value returned by score samples are log likelihood
# use exp to get the original probability density value
probabilities = exp(probabilities)

# print(values[:])
# plot the histogram and pdf
pyplot.hist(sample, bins=50, density=True)
pyplot.plot(values[:], probabilities)
pyplot.savefig('bimodal_kde.png')

print(model.get_params())
