import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile
import scipy.fftpack
from audioClassifier import audioClassifier
import time as t
from scipy.signal import find_peaks
from scipy.signal import argrelextrema
from python_speech_features import mfcc, logfbank
import csv


somzao = np.array([])


wavfile.write('lowRight.wav',8000,somzao)