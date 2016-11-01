/* simulator4.c

Code for Lunar Hopper GDP Pilot Training Simulator, improved to
compute actual forces acting on the hopper and the resulting acceleration
and furtehr improved to simulate oxidiser ullage and thermal effect on fuel
melt rate

Adam Elkins, 08/03/2016
*/

#include<stdio.h>
#include<math.h>

// Declare variables:
float P = 40; // ox tank pressure
float P_demand = 40; // demand pressure, function of throttle setting
float V_N2 = 0.0002; // initial oxidiser tank N2 volume
float dV_N2 = 0; // change in N2 volume due to expulsion of oxidiser from tank
float Gamma = 1.4; // ratio of specific heat capacities
float Const = 0; // P*V^Gamma, adiabatic constant
float oxrate = 0; // oxidiser flow rate
float HTPBRate = 0; // fuel melt rate
float flowrate = 0; // total propellant flow rate
int maxtime = 12; /* length of run */
float elapsed_time = 0; /**/
float dt = 0.02; /* time increment in seconds */
int Isp = 220; /* specific impulse */
float m_fuel = 10; /* mass of oxidiser plus fuel grain */
int m_structure = 28; /* mass of vehicle less propellant */
float dm = 0; /* propellant increment */
float g = 9.81; /* gravity */
float s = 0; /* displacement, spacecraft altitude */
float v = 0; /* spacecraft velocity */
float Thrust = 0;
float Weight = 0;
float Reaction = 0;
float a = 0;

float fuelrate(void) {
	/* I still need to obtain demand pressure */

	if(P_demand > P) {
		P = P_demand; // do nothing for now, normally set P = P_Demand;
	}
	Const = P*pow(V_N2, Gamma);	// Compute adiabatic constant
	oxrate = 0.16*P/40; 	// assume linear relationship between pressure and ox flow rate
	dV_N2 = oxrate*dt/1370;	// find volume of ox used from density and dm_ox
	V_N2 += dV_N2; // increase N2 volume as ox expelled from tank
	P = Const*pow(V_N2, -Gamma); // compute new pressure due to expansion of N2
	

	/*	Compute HTPB rate: */
	HTPBRate = (1 + (0.2/maxtime)*elapsed_time)*(0.16/8.5); // compute HTPB rate from proportion of propellant used and heating effect

	flowrate = oxrate + HTPBRate;
	return flowrate;
}

int main(void) {
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
		fprintf(stderr, "altitude %2.9f velocity %f pressure %f Thrust %f time %f \n", s, v, P, Thrust, elapsed_time);
	}
	/* Print final altitude and velocity */
	fprintf(stderr, "Final altitude %2.9f \nFinal velocity %f \n", s, v);
	return 0;
}
