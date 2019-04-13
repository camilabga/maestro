import matplotlib.pyplot as plt
from sklearn import preprocessing
import numpy as np

file_name = 'reta_vel'
my_data = np.genfromtxt(file_name + '.csv', delimiter=',')
print(my_data[0])


minimum = np.array([ 52., 208., 0.,  0.])
delta = np.array([502., 37., 15.,  2.])

my_data *= delta
print(my_data[0])

my_data += minimum



for data in my_data:

    plt.plot(data[0],data[1],'bo')
    ax = plt.axes()
    ax.arrow(data[0],data[1],data[2],data[3], fc='k', ec='k')

plt.show()

#np.savetxt(file_name + 'test.csv', my_data, delimiter=",",fmt='%3.5f')
