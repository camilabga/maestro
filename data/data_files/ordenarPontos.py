import matplotlib.pyplot as plt
from sklearn import preprocessing
import numpy as np
import sys

file_name = sys.argv[1]
my_data = np.genfromtxt(file_name, delimiter=',')
pontos =  my_data
vetorOrd = []

def distanciaQ(x1,y1,x2,y2):
    return ((x2-x1)**2)+((y2-y1)**2)

def closestPoint(x, y, pontos):
    menor = 10000000000
    cont = 0
    posicao = -1
    for ponto in pontos:
        dist = distanciaQ(x, y, ponto[0], ponto[1])
        if dist < menor:
            menor = dist
            posicao = cont
        cont = cont + 1
    global vetorOrd
    vetorOrd.append(pontos[posicao])

def onclick(event):
    global pontos
    closestPoint(event.xdata, event.ydata, pontos)

fig, ax = plt.subplots()
fig.canvas.mpl_connect('button_press_event', onclick)

plt.plot(pontos[:,0], 480-pontos[:,1],'r.')
plt.show()

outputFile = input('Salvar como (... .csv) :')
np.savetxt(outputFile+'.csv', vetorOrd, delimiter= ',', fmt='%.5f')

    