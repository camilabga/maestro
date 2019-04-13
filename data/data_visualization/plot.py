import matplotlib.pyplot as plt
from sklearn import preprocessing
import numpy as np


file_name = 'reta'
raw_data = np.genfromtxt(file_name + '.csv', delimiter=',')
raw_data =  raw_data[:,1:]

file_name = 'reta_vel'
raw_data_norm = np.genfromtxt(file_name + '.csv', delimiter=',')
#raw_data_norm =  raw_data[:,0:1]

file_name = 'reta-gwr146'
my_data = np.genfromtxt(file_name + '.txt', delimiter=' ')


fig, axes = plt.subplots(2, 1)

ax1, ax2 = axes



ax1.plot(raw_data_norm[:,0],raw_data_norm[:,1],'ro')


for data in my_data:

    ax1.plot(data[0],data[1],'bo')
    ax1.arrow(data[0],data[1],data[2],data[3], fc='k', ec='k')

ax2.plot(raw_data[:,0],raw_data[:,1],'ro')


#plt.show()

minimum = np.array([ 52., 208., 0.,  0.])
delta = np.array([502., 37., 15.,  2.])

#[ 52.,208.,  0.,  0.][502., 37., 15.,  2.]
print(my_data[0])
my_data *= delta
print(my_data[0])

my_data += minimum

for data in my_data:

    ax2.plot(data[0],data[1],'bo')
    ax2.arrow(data[0],data[1],data[2],data[3], fc='k', ec='k')

plt.show()