import processing.serial.*;

/* Note: variables from the arduino are stored in the vars array, and are numbered as:
      vars[0] = altitude (s)          vars[1] = velocity (v)
      vars[2] = acceleration (a)      vars[3] = throttle (Position, p)
      vars[4] = mass of fuel (m_fuel) vars[5] = thrust (T)
      vars[6] = elapsed time (t)
*/

Serial serial;
PFont hack;

// vars used for plotting the graphs, we store the last x and y for vel and alt,
// then update when we successuflly print a new one
float lastAlt = 0; 
float lastVel = 0;
float lastTimeS = 0;
float lastTimeV = 0;

void setup() {
   
   printArray(Serial.list());
   serial = new Serial(this, Serial.list()[0], 115200); // listen for the arduino on 115200

   fullScreen();
   frameRate(60);
   background(30, 30, 30);
   noStroke();
   strokeCap(SQUARE);
   strokeWeight(4);
   fill(255);
   
   textSize(30);
   hack = loadFont("Hack-Regular-32.vlw");
   textFont(hack, 32);
   
   rectMode(CORNERS);
   
   resetDisplay(); // draw an initial display so we have something to look at
}

void draw() {
  if (mousePressed) {
        checkResetPressed(mouseX, mouseY); // check if we should reset
  }
   while (serial.available() > 0) {
      if (mousePressed) {
        checkResetPressed(mouseX, mouseY);
      }
      String ardrep = serial.readString(); // the arduino report string
      if (ardrep != null) {
         if (ardrep == "STOP" || ardrep == "START") {
            resetDisplay(); // if we get a reset code from the adruino reset the display
         }
         eraseRight(); // draw a box over the monitors on the right, so we don't erase
                       // the graphs by calling background()
         drawInstructions();
         
         // extract the variables from the serial 
         String[] varsStr = split(ardrep, ' ');
         float[] vars = new float[varsStr.length];
         for (int i = 0; i < varsStr.length; i++) {
           vars[i] = parseFloat(varsStr[i]);
         }
          
         println(ardrep); // put the vars onto the screen as debug
         
         /*if (vars.length == 0) { // we've reset maybe?
           resetDisplay();
         }*/
         if (3 < vars.length && vars[3] != Float.NaN) { // throttle
           drawThrottle(vars[3]); 
         }
         if (4 < vars.length && vars[4] != Float.NaN) { // fuel level
           drawFuel(vars[4]);
         }
         if (5 < vars.length && vars[5] != Float.NaN) { // thrust level
           drawThrust(vars[5], vars[4]); 
         }
         if (6 < vars.length && vars[6] != Float.NaN) { // the time
           drawElTime(vars[6]); 
           if (vars[0] != Float.NaN) { // if we get a time reading update the graph for alt
              plotAlt(vars[6], vars[0]);
           }
           if (vars[1] != Float.NaN) { // update veloctiy if we have one
              plotVel(vars[6], vars[1]); 
           }
         }
      }
      delay(10); // to prevent flickering screen delay between prints
   }
   
}

void resetDisplay() { // wipe all the data on the screen, and reset the vars for the graph
  background(30, 30, 30);
  plotAxes();
  drawReset();
  drawElTime(0);
  drawInstructions();
  drawThrottle(0);
  drawThrust(0, 0);
  drawFuel(0);
  
  lastAlt = 0;
  lastVel = 0;
  lastTimeS = 0;
  lastTimeV = 0;
}

void drawReset() { // the reset button, in the bottom left
   stroke(255);
   noFill();
   rect(20, 1020, 90, 1060);
   fill(255);
   text("RST", 28, 1050);
}

void drawInstructions() { // the instructions
   String str = "UoS Lunar Hopper\n";
         str += "Pilot Training Simulator\n";
         str += "------------------------\n";
         str += "You have 15 seconds to\n";
         str += "complete the flight. You\n";
         str += "should try and follow \n";
         str += "the grey line on the alt\n";
         str += "plot. If you deviate you\n";
         str += "could crash.\n\n";
         str += "Good luck!";
  text(str, 1400, 300 );
}

void drawTarget() { // the target path to follow
   
}

void plotAxes() { // the axes for the graphs
   stroke(255);
   fill(255);
   text("ALTITUDE", 80, 80);
   line(100, 100, 100, 500);
   line(100, 500, 1000, 500);
   
   for (int i = 0; i < 9; i++) { // 8 divisions
      if (i % 2 == 0) {
        // every even divison draw a number
        textSize(16);
        text(nf(2 - float(i) / 4, 1, 2), 48, 106 + (i * 50));
      }
      line(90, 100 + (i * 50), 100, 100 + (i *50)); // tick line
      textSize(32);
   }
   text("VELOCITY", 80, 580);
   line(100, 600, 100, 1000);
   line(100, 800, 1000, 800);
   
   for (int i = 0; i < 11; i++) { // 10 divisions
      textSize(16);
      text(nf(5 - float(i), 1, 1), 46, 606 + (i * 40)); // draw every number
      line(90, 600 + (i * 40), 100, 600 + (i * 40));    // tick lines
      textSize(32);
   }
   
}
//
void plotAlt(float t, float s) { // draw the altitdue graph
   if (t < 15 && s < 2 && s <= 1.1 * lastAlt && s >= 0.1 * lastAlt) {
      stroke(2, 136, 209);
      strokeWeight(2);
      line(100 + (lastTimeS * 60), 500 - (lastAlt * 200), 100 + (t * 60), 500 - (s * 200)); 
      lastAlt = s;
      lastTimeS = t;
      strokeWeight(4);
   } else {
      noStroke();
   }
}
// && v > -5 && v <= 1.1 * lastVel && v >= 0.9 * lastVel
void plotVel(float t, float v) { // draw the veloctiy graph
    if (t < 15 && v < 5 && v > -5 && v <= 1.1 * lastVel && v >= 0.9 * lastVel) {
        stroke(56, 142, 60);
        strokeWeight(2);
     } else {
        noStroke();
     }
    
    line(100 + (lastTimeV * 60), 800 - (lastVel * 40), 100 + (t * 60), 800 - (v * 40)); 
    lastTimeV = t;
    lastVel = v;
    strokeWeight(4);

   
}

void eraseRight() { // clear the meters on the right
   noStroke();
   fill(30, 30, 30);
   rect(1000, 1080, 1920, 0);
   fill(255);
   stroke(255);
}

void drawElTime(float t) { // show how long since ignition
  
  stroke(255);
  if (t > 12) {
    stroke(206, 28, 8);
    fill(206, 28, 8);
  }
  noFill();
  rect(1230, 120, 1900, 20);
  String time;
  if (t > 15) {
     time = "15.00"; 
  } else {
     time = nf(t, 2, 2); 
  }
  text("MISSION ELAPSED TIME: " + time + "s", 1300, 80);
  fill(255);
  stroke(255);
}

void drawThrottle(float p) {
  drawBarPlot("THROT", p, -10, 10, 1450, 1000, 200);
}

void drawFuel(float m_fuel) {
   drawBarPlot("FUEL", m_fuel, 1, 200, 1600, 1000, 40);
}

void drawThrust(float T, float m_fuel) {
  // draw the thrust as the sum of structure mass (28) and fuel mass
  drawBarPlot("THRST", T, 9.81 * (m_fuel + 28), 380, 1750, 1000, 0.5);
}

void drawBarPlot(String name, float var, float min_warn, float max_warn, float x, float y, float scale) {
  // used to draw the plots for thrust, throttle and fuel
  // check if we're within the warnings
  if (var > max_warn || var < min_warn)
     fill(206, 28, 8);
   text(name, x - 2, y + 40); // put the name underneath the bars
   noStroke();
   rect(x, y, x + 80, y - (scale*var)); // draw the bar
   if (var > max_warn || var < min_warn) // reset if we were in warning mode
     fill(255);
   stroke(255);
   line(x, y, x + 80, y); // draw the bottom axis
   line(x + 80, y, x + 80, y - 200); // now the right axis
   for (int i = 0; i < 6; i++) { // now the ticks, 5 of them
      line(x + 78, y - (i * 40), x + 90, y - (i * 40)); 
   }
}

void checkResetPressed(int x, int y) { // see if the mouse is inside the reset button
  if  (x > 20 && x < 80 && y > 1020 && y < 1060) {
     resetDisplay(); 
  }
}