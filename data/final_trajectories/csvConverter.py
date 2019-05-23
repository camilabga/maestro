import matplotlib.pyplot as plt
from sklearn import preprocessing
import numpy as np
import time


file_name = 'ternario'
raw_data = np.genfromtxt(file_name + '.txt', delimiter=' ')
f=open(file_name + '.txt'   , "r")
if f.mode == 'r':
    header = f.readline()

np.savetxt(file_name + '.csv', raw_data, delimiter=",",fmt='%3.5f',header=header)
