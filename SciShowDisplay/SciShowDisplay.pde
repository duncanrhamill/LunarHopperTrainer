import processing.serial.*;
import grafica.*;

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
float lastS = 0;
float lastV = 0;

// array used to store last vars
float lastVars[];

boolean nozzelMelt = false;
boolean warnTime = false;

GPointsArray sPoints;
GPointsArray vPoints;
GPlot sPlot;
GPlot vPlot;

void setup() {
  lastVars = new float[7]; 
  
   printArray(Serial.list());
   serial = new Serial(this, Serial.list()[0], 115200); // listen for the arduino on 115200

   fullScreen();
   frameRate(30);
   background(30, 30, 30);
   noStroke();
   strokeCap(SQUARE);
   strokeWeight(4);
   fill(255);
   
   textSize(30);
   hack = loadFont("Hack-Regular-32.vlw");
   textFont(hack, 32);
   
   rectMode(CORNERS);
   
   sPlot = new GPlot(this, 100, 100, 900, 400);
   vPlot = new GPlot(this, 100, 600, 900, 400);
   
   float zero[] = {0};
   
   sPoints = new GPointsArray(zero, zero);
   vPoints = new GPointsArray(zero, zero);
   
   sPlot.setPoints(sPoints);
   vPlot.setPoints(vPoints);
   
   sPlot.setXLim(0, 15);
   sPlot.setYLim(0, 5);
   
   vPlot.setXLim(0, 15);
   vPlot.setYLim(-5, 5);
   
   resetDisplay(); 
   
   drawPlot(sPlot);
   drawPlot(vPlot);
}

void draw() {
   background(30);
   
   drawPlot(sPlot);
   drawPlot(vPlot);
   
   drawInstructions();
   
   float[] vars = new float[7];
   boolean useLast = false;
   
   if (mousePressed) {
        checkResetPressed(mouseX, mouseY); // check if we should reset
   }
   if (serial.available() > 0) {
      String ardrep = serial.readString(); // the arduino report string
      if (ardrep != null && trim(ardrep) != null) {
         // extract the variables from the serial 
         String[] varsStr = split(ardrep, ' ');
         
         if (varsStr.length == 7) {
           for (int i = 0; i < varsStr.length; i++) {
             vars[i] = parseFloat(varsStr[i]);
           }
         } else {
            useLast = true;
         }
          
         println(ardrep); // put the vars onto the screen as debug
      } else {
         useLast = true; 
      }
   } else {
      useLast = true; 
   }
   
   if (!useLast) {
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
         if (vars[6] > 12) {
           warnTime = true; 
         }
         if (vars[6] > 15) {
           nozzelMelt = true;
         }
         if (nozzelMelt) {
           drawElTime(15);
         } else {
            drawElTime(vars[6]);  
         }
         
         if (vars[0] != Float.NaN && vars[1] != Float.NaN) { // if we get a time reading update the graph for alt
            plot(vars[6], vars[0], vars[1]);
         }
      }
      lastVars = vars;
   } else {
      if (lastVars[3] != Float.NaN) {
          drawThrottle(lastVars[3]);
      }
      if (lastVars[4] != Float.NaN) {
          drawFuel(lastVars[4]); 
      }
      if (lastVars[4] != Float.NaN && lastVars[5] != Float.NaN) {
          drawThrust(lastVars[5], lastVars[4]); 
      }
      if (lastVars[6] != Float.NaN) {
         if (nozzelMelt) {
           drawElTime(15);
         } else {
            drawElTime(lastVars[6]);  
         }
         if (lastVars[0] != Float.NaN && lastVars[1] != Float.NaN) {
            plot(lastVars[6], lastVars[0], lastVars[1]); 
         }
      }
   }
   
   drawReset();
   //delay(10); // to prevent flickering screen delay between prints
}

void drawPlot(GPlot plot) {
  text("VELOCITY (m/s)", 80, 580);
  text("ALTITUDE (m)", 80, 80);
  plot.beginDraw();
  plot.drawBackground();
  plot.drawXAxis();
  plot.drawYAxis();
  plot.getMainLayer().drawPoints();
  plot.endDraw();
}

void resetDisplay() { // wipe all the data on the screen, and reset the vars for the graph
  background(30, 30, 30);
  for (int i = 0; i < lastVars.length; i++) { 
     lastVars[i] = 0; 
  }
  warnTime = false;
  nozzelMelt = false;
  //drawTarget();
  //plotAxes();
  drawReset();
  drawElTime(0);
  drawInstructions();
  drawThrottle(0);
  drawThrust(0, 0);
  drawFuel(0);
  
  sPoints = new GPointsArray();
  vPoints = new GPointsArray();
  
  sPlot.setPoints(sPoints);
  vPlot.setPoints(vPoints);
  
  drawPlot(sPlot);
  drawPlot(vPlot);
}

void drawReset() { // the reset button, in the bottom left
   stroke(255);
   noFill();
   rect(20, 1020, 90, 1060);
   fill(255);
   text("RST", 27, 1052);
}

void plot(float t, float s, float v) {
    if (t < 15) {
      //if (s <= 1.5*lastS && s >= 0.5*lastS) {
        sPlot.addPoint(t, s);
        lastS = s;
      //}
      //if (v <= 1.5*lastV && v >= 0.5*lastV) {
        vPlot.addPoint(t, v);
        lastV = v;
     // }
      
    }
}

void drawInstructions() { // the instructions
   String str = "UoS Lunar Hopper\n";
         str += "Pilot Training Simulator\n";
         str += "------------------------\n";
         str += "You have 15 seconds to\n";
         str += "complete the flight. You\n";
         str += "should try to hover at\n";
         str += "about 2m in altidude for\n";
         str += "3 seconds at the most.\n";
         str += "If you deviate you could\n";
         str += "crash.\n\n";
         str += "Good luck!";
  text(str, 1400, 250 );
}

void drawTarget() { // the target path to follow
   noFill();
   stroke(80);
   bezier(220, 500, 340, 300, 560, 300, 800, 500);
   stroke(255);
   fill(255);
}

void plotAxes() { // the axes for the graphs
   stroke(255);
   fill(255);
   
   line(100, 98, 100, 500); 
   line(100, 500, 1000, 500); // this happens to be an x axis with each pixel representing 1 frame
   
   for (int i = 0; i < 9; i++) { // 8 divisions
      if (i % 2 == 0) {
        // every even divison draw a number
        textSize(16);
        text(nf(2 - float(i) / 4, 1, 2), 48, 106 + (i * 50));
      }
      line(90, 100 + (i * 50), 100, 100 + (i *50)); // tick line
      textSize(32);
   }
   
   line(100, 598, 100, 1002);
   line(100, 800, 1000, 800);
   
   for (int i = 0; i < 11; i++) { // 10 divisions
      textSize(16);
      text(nf(5 - float(i), 1, 1), 46, 606 + (i * 40)); // draw every number
      line(90, 600 + (i * 40), 100, 600 + (i * 40));    // tick lines
      textSize(32);
   }
   
}

/*
void plotAlt(float t, float s) { // draw the altitdue graph
   if (t < 15 && s < 2 && s <= 1.1 * lastAlt && s >= 0.9 * lastAlt) {
      stroke(2, 136, 209);
   } else {
      noStroke();
   }
   line(100 + lastTimeS, 500 - (lastAlt * 200), 100 + lastTimeS++, 500 - (s * 200)); 
   lastAlt = s;
}
// 
void plotVel(float t, float v) { // draw the veloctiy graph
    // don't plot if we get a random spike in velocity, probably noise
    if (t < 15 && v < 5 && v > -5 && v <= 1.1 * lastVel && v >= 0.9 * lastVel ) {
        stroke(56, 142, 60);
     } else {
        noStroke();
     }
    line(100 + lastTimeV, 800 - (lastVel * 40), 100 + lastTimeV++, 800 - (v * 40)); 
    lastVel = v;
}
*/
void eraseRight() { // clear the meters on the right
   noStroke();
   fill(30, 30, 30);
   rect(1000, 1080, 1920, 0);
   fill(255);
   stroke(255);
}

void drawElTime(float t) { // show how long since ignition
  stroke(255);
  if (t > 12 || warnTime) {
    stroke(206, 28, 8); // warn if greater than 12s
    fill(206, 28, 8);
  }
  noFill();
  rect(1230, 120, 1900, 20);
  String time;
  if (t > 15) {
     time = "15.00"; // if greater than 15s hold it at 15
  } else {
     time = nf(t, 2, 2); // print the time as 00.00 so we don't have flickering digits
  }
  text("MISSION ELAPSED TIME: " + time + "s", 1300, 80);
  fill(255);
  stroke(255);
}

void drawThrottle(float p) {
  // don't warn the pilot about the throttle
  drawBarPlot("THROT", p, -10, 10, 1450, 1000, 200);
}

void drawFuel(float m_fuel) {
  // warn when we have 20% fuel remaining
   drawBarPlot("FUEL", m_fuel, 1, 200, 1600, 1000, 40);
}

void drawThrust(float T, float m_fuel) {
  // warn the pilot if the thrust is less than the weight of the craft, or greater
  // than 380
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