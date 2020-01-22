#include <stdio.h>
#include "actionneur.h"

 int  blink() {

	 digitalWrite(1, 1);		// LED ON
	 return 1;
 }
int offBlink() {

	digitalWrite(1, 0);		// LED OFF
   return 0;
}
