"""plotsimulator.py

Python script to obtain numerical data saved from simulator code
and plot a graph of velocity and altitude against time

Adam Elkins, 05/04/2016"""

import pylab as p

filein = open("simulatordata.txt", 'r')
lines = filein.readlines()
filein.close()
rows = [line.split() for line in lines]

Altitude = [row[0] for row in rows]
Velocity = [row[1] for row in rows]
Time = [row[2] for row in rows]

p.plot(Time, Altitude, 'b', label='Altitude, m')
p.plot(Time, Velocity, 'r', label='Velocity, m/s')
p.legend(loc='upper left')
p.xlabel('Time, s')
p.title('Plot of simulator altitude and velocity against time\n')
p.grid()
p.savefig('Simulatorplot.jpg')
#p.show()
