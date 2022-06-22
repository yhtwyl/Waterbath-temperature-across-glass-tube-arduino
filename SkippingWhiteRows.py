from cProfile import label
from matplotlib import axis
import numpy as np
import matplotlib.pyplot as plt
data = np.genfromtxt('2022-03-20-10-43.txt')
# data1 = np.genfromtxt('2022-02-23-12-32.txt')
# data2 = np.genfromtxt('2022-02-23-12-48.txt')
# data = np.concatenate((data0,data1,data2),axis=0)
# print(my_arr[1:10,0])

DS18B20 = data[:,0]
Therm0 = data[:,1]
Therm1 = data[:,2]
# fig, (ax1, ax2, ax3) = plt.subplots(3,1,sharex=True)
time = np.linspace(0,int(Therm0.size),int(Therm0.size))
""" ax1.plot(time,DS18B20,label = 'DS18B20')
ax1.set(xlabel = 'time (s)',ylabel = 'Temp (degC)')
ax2.plot(time,Therm0,label = 'Therm top')
ax2.set(ylabel = 'Temp (degC)')
ax3.plot(time,Therm1,label = 'Therm bottom')
ax3.set(ylabel = 'Temp (degC)')
ax1.grid()
ax2.grid()
ax3.grid()
plt.show() """

data_init = 0
time = time[data_init:int(time.size)]
# print(time.size)
# plt.plot(time,DS18B20[350:int(DS18B20.size)],label = 'ds18b20')
plt.plot(time,Therm1[data_init:int(Therm1.size)],label = 'therm_bot')
plt.plot(time,Therm0[data_init:int(Therm0.size)],label = 'therm_top')
plt.xlabel('Time (s)')
plt.ylabel('Temp ($^\circ$C)')
plt.minorticks_on()
plt.grid(True,which='both')
plt.legend()
plt.title('Temperature of water at the entry and exit of 4mm(ID) tube')
plt.show()