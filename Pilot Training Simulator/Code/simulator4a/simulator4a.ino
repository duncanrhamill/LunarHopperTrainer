/* simulator.ino
 *  Code for Lunar hopper Pilot Training Simulator: obtain a
 *  throttle setting reading from a potentiometer connected to
 *  analog input and compute resulting vehicle flight velocity and
 *  altitude.
 *  
 *  Adam Elkins, 05/04/2016
 */

// Declare variables and constants here
const int throttlePin = 0; // Declare throttle pin
int velocityPin = 11;  // Pins for output to voltmeters
int altitudePin = 10;
boolean prnt = true; // true to print to serial monitor

float P = 0; // ox tank pressure
float P_demand; // demand pressure, function of throttle setting
float V_N2 = 0.0002; // initial oxidiser tank N2 volume
float dV_N2 = 0; // change in N2 volume due to expulsion of oxidiser from tank
float Gamma = 1.4; // ratio of specific heat capacities
float Const = 0; // P*V^Gamma, adiabatic constant
float oxrate = 0; // oxidiser flow rate
float HDPERate = 0; // fuel melt rate
float flowrate = 0; // total propellant flow rate
const float dt = 0.02; // time step in milliseconds
const int maxtime = 10; // length of run
float elapsed_time = 0; //
float Isp = 0; // specific impulse, 0 for monoprop mode
const int Isp_val = 220; // Value of Isp for biprop mode
float monopropTime = 2; // delay before solid propellant combustion starts
float m_fuel = 4 + 1; // oxidiser plus fuel grain
const int m_structure = 28; // mass of structure
float dm = 0; // propellant increment
const float g = 9.81; // gravity
float s = 0; // displacement, spacecraft altitude
float v = 0; // spacecraft velocity
float Thrust = 0;
float Weight = 0;
float Reaction = 0;
float a = 0;
int startTime = 0;
int endTime = 0;
int delayTime = 0; // variables to compute correct delay time
float throttle;

float fuelrate() {
  /* obtain demand pressure */
  throttle = analogRead(throttlePin)/1023.0;
  P_demand = 40*throttle;
  if(P_demand > P) {
    P = P_demand;
  }
  Const = P*pow(V_N2, Gamma); // Compute adiabatic constant
  oxrate = 0.16*P/40;   // assume linear relationship between pressure and ox flow rate
  dV_N2 = oxrate*dt/1370; // find volume of ox used from density and dm_ox
  V_N2 += dV_N2; // increase N2 volume as ox expelled from tank
  P = Const*pow(V_N2, -Gamma); // compute new pressure due to expansion of N2
  /*  Compute HTPB rate: */
  if(elapsed_time < monopropTime) {
    HDPERate = 0; // Monoprop mode
    Isp = 0;
  }
  else {
    HDPERate = (1 + (0.2/maxtime)*(elapsed_time-monopropTime))*(0.16/8.5); // compute HDPE rate from proportion of propellant used and heating effect
    Isp = Isp_val;
  }
  flowrate = oxrate + HDPERate;
  return flowrate;
}

void setup() {
  //
  if (prnt) {
    Serial.begin(115200);
    Serial.println("==============================================================================================");
    Serial.println();
    Serial.println();
  }
}

void loop() {
  //
  startTime = millis();
  simulator();
  endTime = millis();
  /* to compute dynamic delay:
   *  get millis() at start of loop
   *  get millis at end of loop and subtract to get difference
   *  delayTime = dt - (difference)
   */
  delayTime = int(dt*1000) - (endTime - startTime);
  //Serial.println(delayTime);
  delay(delayTime);
}

void simulator(){
  if (elapsed_time < maxtime) {
    /* Compute sum of forces and accelerations: */
    dm = fuelrate()*dt; /* obtain throttle setting */
    if (m_fuel - dm <= 0) { /* ran out of fuel */
      dm = m_fuel;
      m_fuel = 0;
      }
    else {
      m_fuel -= dm; /* reduce vehicle mass as fuel burnt */
    }

    /* Compute forces acting on vehicle, obtain resultant and 
    acceleration */
    Thrust = Isp*g*dm/dt; /* Exhaust velocity (Isp*g)*mdot */
    Weight = (m_fuel + m_structure)*g;
    if (s <= 0) { /* Vehicle is on ground, compute ground 
      reaction force */
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
      s = 0;  /* s < 0 is unphysical */
      if (v != 0) {
        if (prnt) {
          Serial.print("Landing; impact speed = ");
          Serial.println(v); /* print landing speed */
        }
      }
      v = 0;  /* vehicle has landed */
      a = 0;
    }

    elapsed_time += dt; /* Increment time */
    /* Print vehicle altitude, velocity and time: */
    if (prnt) {
      Serial.print("altitude ");
      Serial.print(s,9);
      Serial.print(" velocity ");
      Serial.print(v,6);
      Serial.print(" acceleration ");
      Serial.print(a);
      Serial.print(" throttle ");
      Serial.print(analogRead(throttlePin)/1023.0);
      Serial.print(" fuel ");
      Serial.print(m_fuel);
      Serial.print(" Pressure ");
      Serial.print(P);
      Serial.print(" Thrust ");
      Serial.print(Thrust);
      Serial.print(" time ");
      Serial.print(elapsed_time, 6);
      Serial.print("\n");
    }
    
// Output altitude and velocity to analog pins for voltmeters
    if(s < 5) {
      analogWrite(altitudePin, s*255/5.0);
    }
    else {
      if(s >= 5) {
        analogWrite(altitudePin, 255);
      }
    }
    
    if(v > -2.5 && v < 2.5) {
      analogWrite(velocityPin, (v + 2.5)*255/5.0);
    }
    else {
      if(v >= 2) {
        analogWrite(velocityPin, 255);
      }
      if(v <= -2) {
        analogWrite(velocityPin, 0);
      }
    }
  }
}

