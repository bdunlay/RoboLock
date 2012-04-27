/** RoboLock
 * LED.C
 *
 * Source file for GPIO control, configuration and usage of LEDs on the RoboLock board
 *
 * v1.0 - 4/22/2012
 */

#include "LPC23xx.h"
#include "LCD.h"
#include "type.h"
#include "target.h"

volatile BYTE I2CMasterBuffer[BUFSIZE];
 BYTE buff;
 void delay();

void initLCD(void) {
I2CInit(0);

}

void clearLCD(void) {
	buff = 0x01;
	I2CMasterBuffer[0] = buff;
	I2C1MasterHandler();
}


void printLCD(BYTE val) {
	buff = val;
	I2CMasterBuffer[0] =buff;
	I2C1MasterHandler();
}


void testLCD() {
		int i;
		i = 0;
		clearLCD();

		for (i = 0; i < 10; i++) {
		printLCD(0x42);
		delay();
		printLCD(0x43);
		delay();
		}
}
