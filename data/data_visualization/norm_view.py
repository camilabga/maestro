import matplotlib.pyplot as plt
from sklearn import preprocessing
import numpy as np


file_name = 'quaternario'
my_data = np.genfromtxt(file_name + '.csv', delimiter=',')
#my_data =  my_data[:,1:]
print(my_data[0])
my_data =  my_data[:,0:2]
#my_data[:,1] = 480 - my_data[:,1] 
print(len(my_data))


#plt.axis([0, 640, 0, 480])

cutted_data = np.empty([1, 2])
print(cutted_data)
start = False
for data in my_data:

    plt.plot(int(data[0]),480-int(data[1]),'r.')
    #plt.text(int(data[0]),(data[1]),str(data[0])+' , '+str(data[1]))
    #plt.arrow(data[0],data[1],data[2],data[3], fc='k', ec='k')
    plt.show(block=False)
    if start:
        cutted_data = np.insert(cutted_data, len(cutted_data), [data[0],data[1]],axis=0)
    a = input()
    if a is '0':
        break
    if a is '1':
        start = True

my_data = cutted_data[1:,:]
print('*'*80)
print(my_data[0])


vel = np.empty([len(my_data), 2])

for i in range(len(my_data)):
    if(i != len(my_data)-1):
        dx = my_data[i+1,0] - my_data[i,0] 
        dy = my_data[i+1,1] - my_data[i,1]
        #if(abs(dx) < 200 and abs(dy) < 200):
        vel[i] = [dx,dy]
        #else:
        #    vel[i] = [0,0]

my_data = np.append(my_data,vel,axis=1)

#fig, axes = plt.subplots(2, 1)

#ax1, ax2 = axes
print('luiz',np.argwhere(my_data == 0))
print(my_data)
for data in my_data:

    plt.plot(int(data[0]),480-int(data[1]),'r.')
    #plt.text(int(data[0]),(data[1]),str(data[0])+' , '+str(data[1]))
    
    plt.arrow(data[0],480-data[1],data[2],-data[3], fc='k', ec='k')
plt.axis([0, 640, 0, 480])

plt.show()




#minimum = np.absolute(my_data).min(axis=0)
minimum = np.array([ 74.,128.,  0.,  0.])
#maximum = np.absolute(my_data).max(axis=0)

#delta = maximum - minimum
delta = np.array([328.,272.,108.,184.])
header = 'X, Y, VelX, VelY |' + np.array2string(minimum, separator=';') + np.array2string(delta, separator=';')


#np.savetxt(file_name + '_unorm_vel.csv', my_data, delimiter=",",fmt='%3.5f',header=header)


my_data -= minimum



my_data /= delta
print(minimum,delta)
print(header)
'''
for data in my_data:

    ax2.plot(data[0],data[1],'bo')
    ax2.arrow(data[0],data[1],data[2],data[3], fc='k', ec='k')

plt.axis([0, 1920, 0, 1080])
'''
plt.show()

np.savetxt(file_name + '_unique_vel.csv', my_data, delimiter=",",fmt='%3.5f',header=header)


