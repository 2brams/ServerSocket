#include <stdio.h>
#include "actionneur.h"

 void blink() {

	 digitalWrite(0, 1);		// LED ON
//	if (wiringPiSetup() == -1)
//		return 1;

//	pinMode(0, OUTPUT);		// pin 11 du connecteur HE10 configurée en sortie
//
//	while (1) {
//		digitalWrite(0, 1);		// LED ON
//		delay(500);			// tempo 500ms
//		digitalWrite(0, 0);		// LED OFF
//		delay(500);			// tempo 500ms
//	}

}
void offBlink() {

	digitalWrite(0, 0);		// LED OFF

}
