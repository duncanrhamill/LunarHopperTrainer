/* A bit of code to implement the start and reset pins for the simulator
 * based on http://www.instructables.com/id/two-ways-to-reset-arduino-in-software/step2/using-just-software/
 * Adam Elkins, 01/04/2016, ae3g10@soton.ac.uk
  */

// pin declarations:
int startButtonPin;
int resetButtonPin;
bool startMode = LOW;

// inside setup():
void setup() {
	pinMode(startButtonPin, INPUT);
	pinmode(resetButtonPin, INPUT);
}

// Declare reset function:
// source: http://www.instructables.com/id/two-ways-to-reset-arduino-in-software/step2/using-just-software/
void(* resetFunc) (void) = 0; //declare reset function @ address 0

// inside loop():
void loop() {
	if(digitalRead(resetButtonPin) = HIGH) {
		resetFunc(); // reset the arduino if reset button is pushed
	}
	if(startMode = LOW) {
		startMode = digitalRead(startPin);
	}
	else {
		simulator();
	}
}
