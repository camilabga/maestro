import matplotlib.pyplot as plt
from sklearn import preprocessing
import numpy as np


file_name = 'new_movement'
my_data = np.genfromtxt(file_name + '.csv', delimiter=',')
#my_data =  my_data[:,1:]
print(my_data[0])
my_data =  my_data[:,1:3]
#my_data[:,1] = 480 - my_data[:,1] 
print(my_data)


#plt.axis([0, 640, 0, 480])

plt.plot(my_data[:,0],480-my_data[:,1],'r.')
    #plt.text(int(data[0]),(data[1]),str(data[0])+' , '+str(data[1]))
    #plt.arrow(data[0],data[1],data[2],data[3], fc='k', ec='k')
plt.show()
    