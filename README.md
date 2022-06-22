# Waterbath-temperature-control-and-measurement-across-glass-tube-using-arduino
File "WaterBathAndTemperatureRiseAcrossGlassTube.ino" controls the temperature of water bath at the desired set temperature. Program on PID control has been imported from Electronoobs[1] and implemented herein for temperature control of water bath. 
First the whole system was modelled using the notes of Dr. Barry Johnson[2] and the system equations were used for finding the appropriate values of PID constants with the help of SIMULINK.
PlottingLiveData.py does a live ploting of water bath temperature and other sensors mounted at positions P1 and P2 (schematic picture of the whole set-up showing the locations of sensors ExperimentalSetup.jpg) and also optionally saves the data. The saved data can be later on ploted using SkippingWhiteSpace.py program.

First the arduino program WaterBathAndTemperatureRiseAcrossGlassTube.ino is run and then PlottingLiveData.py is run from the same directory. You may have to change to COM port according to your local computer system.

# References:
1. https://electronoobs.com/eng_arduino_tut39.php
2. Dr. Barry S. Johnston. Process dynamics, operations, and control (10.450). Spring 2006. Massachusetts Institute of Technology: MIT OpenCouseWare, https://ocw.mit.edu/. License: Creative Commons BY-NC-SA
