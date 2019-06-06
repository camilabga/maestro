import matplotlib.pyplot as plt
from sklearn import preprocessing
import numpy as np
import time


file_name = 'quaternario'
raw_data = np.genfromtxt(file_name + '.csv', delimiter=',')
#raw_data =  raw_data[:,1:]


raw_data_norm = np.genfromtxt(file_name + '_unique_vel.csv', delimiter=',')
#raw_data_norm =  raw_data[:,0:1]

#file_name = '../final_trajectories/binario2'
file_name = 'quaternario'
my_data = np.genfromtxt(file_name + '.txt', delimiter=' ')


#fig, axes = plt.subplots(2, 1)

#ax1, ax2 = axes

#plt.ion()
plt.axis([-0.2, 1.3, 478.9, 480.1])

plt.plot(raw_data_norm[:,0],480 - raw_data_norm[:,1],'r.')

'''
for data in my_data:

    plt.plot(data[0],480 - data[1],'b.')
    plt.arrow(data[0],480 - data[1],data[2],-data[3], fc='k', ec='k')
plt.axis([-1,1,479,481])'''
plt.show()

#ax2.plot(raw_data[:,0],raw_data[:,1],'ro')


#plt.show()

minimum = np.array([ 74.,128.,  0.,  0.])
delta = np.array([328.,272.,108.,184.])

#[ 52.,208.,  0.,  0.][502., 37., 15.,  2.]
print(my_data[0])
my_data *= delta
raw_data_norm*=delta
print(my_data[0])

my_data += minimum
raw_data_norm+=minimum

plt.axis([0, 640, 0, 480])

#plt.plot(raw_data_norm[:,0], 480 - raw_data_norm[:,1],'r.')
plt.plot(raw_data[:,0], 480 - raw_data[:,1],'r.')
for data in my_data:
    

    plt.plot(data[0], 480 - data[1],'bo')
    plt.arrow(data[0],480 - data[1],data[2], -data[3], fc='k', ec='k')

plt.show()
    #time.sleep(1)
