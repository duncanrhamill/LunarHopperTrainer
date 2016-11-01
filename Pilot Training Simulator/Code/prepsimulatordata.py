"""prepsimulatordata.py

Python script to adapt data from later stage of simulator
development to use in plotsimulator.py

Adam Elkins, 05/04/2016"""


file = open("Simulator test excerpt.txt", "r")
lines = file.readlines()
file.close()

fout = open('fout.txt','w')
for line in lines:
	words = line.split()
	altitude = words[1]
	velocity = words[3]
	time = words[15]
	fout.write(altitude + ' ' + velocity + ' ' + time + '\n')
