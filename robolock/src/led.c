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

	//	PINMODE3 = 0xAAAAAAAA;

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

	FIO1CLR2 = val<<2;
	FIO1CLR3 = val>>6;

}

void delay() {
	int a;
	int b;
	int c;

	for (a = 0; a < 5; a++) {
		for (b = 0; b < 32767; b++) {
		}
	}
}

void testLED() {
		int i, j;
		j = 0;
		i = 0;

		for (i = 0; i < 10; i++) {
		printLED(0xAA);
		delay();
		printLED(0x55);
		delay();
		}

		for (i = 0; i < 3; i++) {
			for (j = 1; j < 255; j |= j*2) {
				delay();
				printLED(j);
			}

			for (j = 1; j < 255; j |= j*2) {
				delay();
				printLED(~j);
			}
		}
}
