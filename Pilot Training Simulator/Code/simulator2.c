/* simulator2.c

Skeleton code for Lunar Hopper GDP Pilot Training Simulator
Improved to prevent negative (unphyiscal) values of altitude or propellant mass
and to remove redundant code

Adam Elkins, 03/02/2016

*/

#include<stdio.h>

float fuelrate(void) {
	/* return fuel consumption rate (i.e. throttle setting) */
	float rate = 0.17; /* kg/s, simulate constant throttle setting */
	return rate;
}

int main(void) {
	/* Declare variables */
	int maxtime = 12; /* length of run */
	float elapsed_time = 0; /**/
	float dt = 0.02; /* time increment in seconds */
	int Isp = 220; /* specific impulse */
	float m_fuel = 4 + 6; /* oxidiser plus fuel grain */
	int m_structure = 38 - m_fuel;
	float dm = 0; /* propellant increment */
	float g = 9.81; /* gravity */
	float s = 0; /* displacement, spacecraft altitude */
	float v = 0; /* spacecraft velocity*/
	float dv = 0; /* velocity imcrement */
	float dv_thrust = 0; /* dv due to thrust */
	float R = 0; /* ground reaction force */
	
	while (elapsed_time < maxtime) {
		/* Compute sum of forces and accelerations: */
		dm = fuelrate()*dt;	/* obtain throttle setting */
		if (m_fuel - dm <= 0) {	/* ran out of fuel */
			dm = m_fuel;
			m_fuel = 0;
			}
		else {
			m_fuel -= dm;	/* reduce vehicle mass as fuel burnt */
		}

		/* Compute dv due to thrust: */
		dv_thrust = (Isp*g*dm)/(m_fuel + m_structure);
		if (s <= 0) {	/* vehicle on ground */
			if (dv_thrust < g*dt) {	/* vehicle not lifting off */
				R = g*dt - dv_thrust;	/* calculate ground reaction force */
			}
		}
		else { /* Vehicle in flight */
			R = 0; /* Zero reaction force */
		}

		dv = dv_thrust - g*dt + R; /* Calculate velocity increment */
		v += dv;	/* increment vehicle velocity */

		if (s + v*dt < 0) { /* Landing or on ground */
			s = 0;	/* s < 0 is unphysical */
			if (v != 0) {
				fprintf(stderr, "Landing; impact speed = %f\n", v); /* print landing speed */
			}
			v = 0;	/* vehicle has landed */
		}
		else {	/* Vehicle in flight */
			s += v*dt;	/* Increment vehicle altitude */
		}

		elapsed_time += dt; /* Increment time */
		/* Print vehicle altitude, velocity and time: */
		fprintf(stdout, " %2.9f\t%f\t%f\n", s, v, elapsed_time);
		fprintf(stderr, "altitude %2.9f velocity %f time %f \n", s, v, elapsed_time);
	}
	/* Print final altitude and velocity */
	fprintf(stderr, "Final altitude %2.9f \nFinal velocity %f \n", s, v);
	return 0;
}
