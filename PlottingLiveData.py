# Importing Libraries
from base64 import decode
from datetime import datetime
from os import device_encoding, read
from turtle import color, title
from matplotlib import markers
import matplotlib.pyplot as plt
import serial
import time
import numpy as np
import winsound
fig, (ax1,ax2,ax3) = plt.subplots(3,1,sharex=True)
a, = ax1.plot(1,1)
b, = ax2.plot(1,1)
c, = ax3.plot(1,1)
ax1.grid()
ax2.grid()
ax3.grid()
ax1.set(ylabel = 'Temp (degC)',title = 'Water bath')
ax2.set(ylabel = 'Temp (degC)',title = 'At the entry')
ax3.set(xlabel = 'time (s)',ylabel = 'Temp (degC)',title = 'At the exit')
plt.pause(0.01)

""" if True:
    # cache the background
    background = fig.canvas.copy_from_bbox(ax.bbox) """

arduino = serial.Serial(port='COM3', baudrate=115200,timeout = 2)

presentime = datetime.now()
filename = presentime.strftime('%Y-%m-%d') + '-' + str(presentime.hour) + '-' + str(presentime.minute) + '.txt'
file = open(filename,'w')

# file.write(" NOW THE TIME IS:" + str(presentime) + '\n')

""" def ReadAndSaveFile():
    data = arduino.readline()
    decoded_values = str(data[0:len(data)].decode("utf-8"))
    if decoded_values:
        file.write(decoded_values)
        if float(decoded_values) > 38.00:
            frequency = 2500  # Set Frequency To 2500 Hertz
            duration = 1000  # Set Duration To 1000 ms == 1 second
            winsound.Beep(frequency, duration) """


def ReadAndUpdate():
    data = arduino.readline()
    decoded_values = str(data[0:len(data)].decode("utf-8"))
    # print(decoded_values)
    if len(decoded_values) == 19:
        # print(len(decoded_values))
        file.write(decoded_values)
        T1,T2,T3 = decoded_values.split('\t')
        if float(decoded_values[:decoded_values.index("\t")]) > 41.00:
            frequency = 2500  # Set Frequency To 2500 Hertz
            duration = 1000  # Set Duration To 1000 ms == 1 second
            winsound.Beep(frequency, duration)
        
        return(T1,T2,T3)

    return(0,0,0)


time_start = time.time()
elapsed_time = 0
ds18b20 = []
therm_top = []
therm_bottom = []
time_len = []
time_count = 0

while elapsed_time < 1200:
    T1, T2, T3 = ReadAndUpdate()
    if (float(T1) > 10.0) and  (float(T2) > 10.0): #and (float(T3) > 10.0) :
        ds18b20.append(float(T1))
        ds18b20 = ds18b20[-20:]
        therm_top.append(float(T2))
        therm_top = therm_top[-20:]
        therm_bottom.append(float(T3) - 0.1895)
        therm_bottom = therm_bottom[-20:]
        time_len.append(time_count)
        time_len = time_len[-20:]
        a.remove()
        b.remove()
        c.remove()
        a, = ax1.plot(time_len,ds18b20,color="#191970",marker = 'o',ms=10,mfc='orange',mec='red',markevery = [len(ds18b20)-1])
        b, = ax2.plot(time_len,therm_top,color="#5D3FD3",marker = 'o',ms=10,mfc='orange',mec='red',markevery = [len(therm_top)-1])
        c, = ax3.plot(time_len,therm_bottom,color = "#89CFF0",marker = 'o',ms=10,mfc='orange',mec='red',markevery = [len(therm_bottom)-1])
        ax1.set_xlim([min(time_len), max(time_len) + 2])
        ax1.set_ylim([min(ds18b20) - 1, max(ds18b20) + 1])
        ax2.set_ylim([min(therm_top) - 1, max(therm_top) + 1])
        ax3.set_ylim([min(therm_bottom) - 1, max(therm_bottom) + 1])
        plt.pause(0.5)
        elapsed_time = time.time() - time_start
        time_count += 1
plt.show()
file.close()