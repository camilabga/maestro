import matplotlib.pyplot as plt
import numpy as np



my_data = np.genfromtxt('../sample00_square_00_amo.csv', delimiter=',')
print(len(my_data))



for i in range(len(my_data)):

    plt.plot(my_data[i,1],my_data[i,2],'bo')
    ax = plt.axes()
    if(i != len(my_data)-1):
        dx = my_data[i+1,1] - my_data[i,1] 
        dy = my_data[i+1,2] - my_data[i,2]
        ax.arrow(my_data[i,1],my_data[i,2], dx, dy, head_width=0.03, head_length=0.05,head_starts_at_zero=False, fc='r', ec='r')

plt.ylabel('some numbers')


#np.savetxt("foo.csv", a, delimiter=",")
plt.show()