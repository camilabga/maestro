import matplotlib.pyplot as plt
from sklearn import preprocessing
import numpy as np

file_name = 'quat'
my_data = np.genfromtxt(file_name + '.csv', delimiter=',')
my_data =  my_data[:,1:]


vel = np.empty([len(my_data), 2])

for i in range(len(my_data)):
    if(i != len(my_data)-1):
        dx = my_data[i+1,0] - my_data[i,0] 
        dy = my_data[i+1,1] - my_data[i,1]
        vel[i] = [dx,dy]

my_data = np.append(my_data,vel,axis=1)

fig, axes = plt.subplots(2, 1)

ax1, ax2 = axes

for data in my_data:

    ax1.plot(my_data[:,0],my_data[:,1],'ro')
    ax1.arrow(data[0],data[1],data[2],data[3], fc='k', ec='k')
plt.axis([0, 1920, 0, 1080])


minimum = np.absolute(my_data).min(axis=0)
maximum = np.absolute(my_data).max(axis=0)
delta = maximum - minimum
header = 'X, Y, VelX, VelY |' + np.array2string(minimum, separator=';') + np.array2string(delta, separator=';')


#np.savetxt(file_name + '_unorm_vel.csv', my_data, delimiter=",",fmt='%3.5f',header=header)


my_data -= minimum



my_data /= delta
print(minimum,delta)
print(header)

for data in my_data:

    ax2.plot(data[0],data[1],'bo')
    ax2.arrow(data[0],data[1],data[2],data[3], fc='k', ec='k')

plt.axis([0, 1920, 0, 1080])

plt.show()

np.savetxt(file_name + '_vel.csv', my_data, delimiter=",",fmt='%3.5f',header=header)


