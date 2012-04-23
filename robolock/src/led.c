/** RoboLock
 * LED.C
 *
 * Source file for GPIO control, configuration and usage of LEDs on the RoboLock board
 *
 * v1.0 - 4/22/2012
 */

#include "LPC23xx.h"
#include "led.h"
#include "type.h"
#include "target.h"

// set LED GPIOs as output
void initLED(void) {
	PINSEL2 = 0x00;
	PINSEL3 = 0x00; // (Re)Set to GPIO

	FIO1DIR2 = 0xFC;
	FIO1DIR3 = 0x03;
	clearLED();

}

// set all GPIOs high
void clearLED(void) {
	FIO1SET2 = 0xFC;
	FIO1SET3 = 0x03;
}


void printLED(unsigned int val) {

	clearLED();

	int i;

	for (i = 0; i < 8; i++) {
		if (i < 6)
			FIO2CLR |= ((val & (0x1 << i)) << 2);
		else
			FIO3CLR |= ((val & (0x1 << i)) >> 6);
	}
}
