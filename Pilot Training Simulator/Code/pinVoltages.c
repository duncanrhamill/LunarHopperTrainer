/* Compute voltages to output velocity and altitude to voltmeters */

int velocityPin = 5;
int altitudePin = 6;

if(a <= 5) {
	analogWrite(altitudePin, a);
}
else {
	analogWrite(altitudePin, 5);
}

if(v <= 2 && v >= -2) {
	velocityPin = (v + 2.5)*1.25;
}
else {
	if(v > 2) {
		analogWrite(velocityPin, 2.5);
	}
	if(v < -2) {
		analogWrite(velocityPin, -2.5);
	}
}