import matplotlib.pyplot as plt
from sklearn import preprocessing
import numpy as np
import time
import sys


file_name = sys.argv[1]
my_data = np.genfromtxt(file_name, delimiter=',')
plt.plot(my_data[:,1], 480 - my_data[:,2],'r.')
plt.show()