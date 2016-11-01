/* simulator3a.c

Code for Lunar Hopper GDP Pilot Training Simulator, improved to
compute actual forces acting on the hopper and the resulting acceleration

Adam Elkins, 08/02/2016

*/
#include<stdio.h>

float fuelrate(void) {
	/* return fuel consumption rate (i.e. throttle setting) */
	float rate = 0.18; /* kg/s, simulate constant throttle setting */
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
	float v = 0; /* spacecraft velocity */
	float Thrust = 0;
	float Weight = 0;
	float Reaction = 0;
	float a = 0;

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

		/* Compute forces acting on vehicle, obtain resultant and acceleration */
		Thrust = Isp*g*dm/dt; /* Exhaust velocity (Isp*g)*mdot */
		Weight = (m_fuel + m_structure)*g;
		if (s <= 0) { /* Vehicle is on ground, compute ground reaction force */
			if (Thrust < Weight) {
				Reaction = Weight - Thrust;
			}
		}
		else {
			Reaction = 0;
		}

		/* compute acceleration: sum of forces divided by mass */
		a = (Thrust + Reaction - Weight)/(m_fuel + m_structure);
		/* Compute altitude s = s0 + u*t + 0.5*a*t^2: */
		s += v*dt + 0.5*a*dt*dt;
		/* Compute velocity v = u + a*t: */
		v += a*dt;

		if (s <= 0) { /* Landing or on ground */
			s = 0;	/* s < 0 is unphysical */
			if (v != 0) {
				fprintf(stderr, "Landing; impact speed = %f\n", v); /* print landing speed */
			}
			v = 0;	/* vehicle has landed */
		}
		elapsed_time += dt; /* Increment time */
		fprintf(stdout, " %2.9f\t%f\t%f\n", s, v, elapsed_time);
		fprintf(stderr, "altitude %2.9f velocity %f acceleration %f time %f \n", s, v, a, elapsed_time);
	}
	/* Print final altitude and velocity */
	fprintf(stderr, "Final altitude %2.9f \nFinal velocity %f \n", s, v);
	return 0;
}
