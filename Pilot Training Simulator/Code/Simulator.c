/* Skeleton code for Lunar Hopper GDP Pilot Training Simulator

Adam Elkins, 28/01/2016

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
	float thrust = 0; /* thrust */
	float R = 0; /* ground reaction force */
	int Reaction = 0; /* reaction force required? */
	
	while (elapsed_time < maxtime) {

		/* determine vehicle state: */
		if (s == 0 && v == 0) {	/* vehicle on ground */
			Reaction = 1;	/* compute reaction force */
		}
		/* else-if statements? */
		if (s <= 0 ) {	/* vehicle landed */
			s = 0; /* s < 0 is unphysical */
			/* print landing velocity */
			v = 0;	/* vehicle has landed */
			Reaction = 1;	/* compute reaction force */
		}

		if (s > 0) {	/* vehicle in flight */
			Reaction = 0;	/* vehicle flying, no ground reaction */
		}

		/* Compute sum of forces and accelerations: */
		dm = fuelrate()*dt;	/* simulate constant throttle setting */
		if (m_fuel > 0) {
			m_fuel -= dm;	/* reduce vehicle mass as fuel burnt */
		}
		else {
			if (m_fuel <= 0) {	/* ran out of fuel */
				m_fuel = 0;
				dm = 0;
			}
		}
		thrust = (Isp*g*dm)/(m_fuel + m_structure);
		if (s <= 0) {	/* vehicle on ground */
			if (thrust < g*dt) {	/* vehicle not lifting off */
				R = g*dt - thrust;	/* calculate ground reaction force */
			}
		}
		else {
			R = 0;
		}

		dv = thrust - g*dt + R;
		v += dv;
		s += v*dt;	/* I need to double-check this */
		elapsed_time += dt;
		printf("altitude %f velocity %f time %f \n", s, v, elapsed_time);
	}
	return 0;
}
