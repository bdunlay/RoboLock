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
#include "common.h"
#include "led.h"

extern volatile DWORD I2CCount;
extern volatile BYTE I2CMasterBuffer[BUFSIZE];
extern volatile DWORD I2CCmd, I2CMasterState;
extern volatile DWORD I2CReadLength, I2CWriteLength;

volatile BYTE I2CMasterBuffer[BUFSIZE];
BYTE buff;

void initLCD(void) {

	I2CInit(I2CMASTER);  // start master

}

void backlightLCD(void){
	FIO1DIR3 |= 0x04;
	PINSEL1 &= ~0x00008000;
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
	  I2CWriteLength = 3;
	  I2CReadLength = 0;
	  I2CMasterBuffer[0] = LCD_ADDR;
	  I2CMasterBuffer[1] = 0x38;//LCD_CONFIG;
	  I2CMasterBuffer[2] = 0x0F;//0x01;
	  I2CMasterBuffer[3] = 0x26;
	  /* configuration value, no change from default */
	  I2CCmd = LCD_CONFIG;
	  printLED(0x02);
	  busyWait(100);
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
		backlightLCD();

		busyWait(100);
		lcdStart();
		busyWait(10000);
//		clearLCD();
//		//printLED(0x06);
//		busyWait(100);
//			printLED(0x07);
//			busyWait(100);
//			printLCD(0x42);
//			busyWait(100);
//			printLCD(0x43);
//			busyWait(1000);

}

void lcdStart(){
	int i=0;
		  for ( i = 0; i < BUFSIZE; i++ )	/* clear buffer */
		  {
			I2CMasterBuffer[i] = 0;
		  }
		  I2CWriteLength = 9;
		  I2CReadLength = 0;
		  I2CMasterBuffer[0] = LCD_ADDR;
		  I2CMasterBuffer[1] = 0x00;
		  I2CMasterBuffer[2] = 0x38;
		  I2CMasterBuffer[3] = 0x39;
		  I2CMasterBuffer[4] = 0x14;
		  I2CMasterBuffer[5] = 0x78;
		  I2CMasterBuffer[6] = 0x5E;
		  I2CMasterBuffer[7] = 0x6D;
		  I2CMasterBuffer[8] = 0x0C;
		  I2CMasterBuffer[9] = 0x06;


		  /* configuration value, no change from default */
		  I2CCmd = LCD_CONFIG;
		  I2CEngine();
}
