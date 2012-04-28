/** RoboLock
 * LED.C
 *
 * Source file for GPIO control, configuration and usage of LCD on the RoboLock board
 *
 * v1.0 - 4/22/2012
 */

#include "LPC23xx.h"
#include "LCD.h"
#include "type.h"
#include "target.h"

extern volatile DWORD I2CCount;
extern volatile BYTE I2CMasterBuffer[BUFSIZE];
extern volatile DWORD I2CCmd, I2CMasterState;
extern volatile DWORD I2CReadLength, I2CWriteLength;

volatile BYTE I2CMasterBuffer[BUFSIZE];
 BYTE buff;
 void delay();

void initLCD(void) {
I2CInit(I2CMASTER);
FIO1DIR3 |= 0x04;
PINSEL1 = 0x00;
FIO1CLR3 |= 0x04;

}

void clearLCD(void) {
//	e.g. Start, DevAddr(W), WRByte1...WRByteN, Repeated-Start, DevAddr(R),
//	  RDByte1...RDByteN Stop. The content of the reading will be filled
//	  after (I2CWriteLength + two devaddr) bytes.

	  /* Configure temp register before reading */
	int i=0;
	  for ( i = 0; i < BUFSIZE; i++ )	/* clear buffer */
	  {
		I2CMasterBuffer[i] = 0;
	  }
	  I2CWriteLength = 2;
	  I2CReadLength = 0;
	  I2CMasterBuffer[0] = LCD_ADDR;
	  I2CMasterBuffer[1] = LCD_CONFIG;
	  I2CMasterBuffer[2] = 0x01;	/* configuration value, no change from
									default */
	  I2CCmd = LCD_CONFIG;
	  I2CEngine();
}


void printLCD(BYTE val) {
	int i=0;
	 for ( i = 0; i < BUFSIZE; i++ )	/* clear buffer */
		  {
			I2CMasterBuffer[i] = 0;
		  }
		  I2CWriteLength = 2;
		  I2CReadLength = 0;
		  I2CMasterBuffer[0] = LCD_ADDR;
		  I2CMasterBuffer[1] = LCD_CONFIG;
		  I2CMasterBuffer[2] = 0x0F;	/* configuration value, no change from
										default */
		  I2CCmd = LCD_CONFIG;
		  I2CEngine();
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
