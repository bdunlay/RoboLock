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
	 // busyWait(100);
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
			  I2CMasterBuffer[1] = 0x40;
			  I2CMasterBuffer[2] = val;
			  I2CEngine();
}


void testLCD() {
		volatile int i;
		backlightLCD();

	//	busyWait(100);
		lcdInit();
		for(i=0;i<200;i++);
		lcdClear();
		for(i=0;i<200;i++);
		  printLCD(0x48); //H
		  printLCD(0x65); //e
		  printLCD(0x6C); //l
		  printLCD(0x6C); //l
		  printLCD(0x6F); //0
		  printLCD(0x20); //_
		  printLCD(0x57); //W
		  printLCD(0x6F); //o
		  printLCD(0x72);//r
		  printLCD(0x6C);//l
		  printLCD(0x64);//d
		  printLCD(0x3F);//?
		//busyWait(1000);

}

void lcdInit(){
	int i=0;
		  for ( i = 0; i < BUFSIZE; i++ )	/* clear buffer */
		  {
			I2CMasterBuffer[i] = 0;
		  }
		  I2CWriteLength = 10;
		  I2CReadLength = 0;
		  I2CMasterBuffer[0] = LCD_ADDR;
		  I2CMasterBuffer[1] = 0x00;
		  I2CMasterBuffer[2] = 0x38;
		  I2CMasterBuffer[3] = 0x39;
		  I2CMasterBuffer[4] = 0x14;
		  I2CMasterBuffer[5] = 0x78;
		  I2CMasterBuffer[6] = 0x5E;
		  I2CMasterBuffer[7] = 0x6D;
		  I2CMasterBuffer[8] = 0x0F;  //0x0C
		  I2CMasterBuffer[9] = 0x01;
		  I2CMasterBuffer[10] = 0x06;


		  /* configuration value, no change from default */
		  I2CCmd = LCD_CONFIG;
		  I2CEngine();
}

void lcdClear(){
	int i=0;
		  for ( i = 0; i < BUFSIZE; i++ )	/* clear buffer */
		  {
			I2CMasterBuffer[i] = 0;
		  }
		  I2CWriteLength = 3;
		  I2CReadLength = 0;
		  I2CMasterBuffer[0] = LCD_ADDR;
		  I2CMasterBuffer[1] = 0x00;
		  I2CMasterBuffer[2] = 0x00;
		  I2CMasterBuffer[3] = 0x01;
		  /* configuration value, no change from default */
		 // I2CCmd = LCD_CONFIG;
		  I2CEngine();
}

void lcdShow(){
	int i=0;
		  for ( i = 0; i < BUFSIZE; i++ )	/* clear buffer */
		  {
			I2CMasterBuffer[i] = 0;
		  }
		  I2CWriteLength = 13;
		  I2CReadLength = 0;
		  I2CMasterBuffer[0] = LCD_ADDR;
		  I2CMasterBuffer[1] = 0x40;
		  I2CMasterBuffer[2] = 0x48; //H
//		  I2CMasterBuffer[3] = 0x65; //e
//		  I2CMasterBuffer[4] = 0x6C; //l
//		  I2CMasterBuffer[5] = 0x6C; //l
//		  I2CMasterBuffer[6] = 0x6F; //0
//		  I2CMasterBuffer[7] = 0x20; //_
//		  I2CMasterBuffer[8] = 0x57; //W
//		  I2CMasterBuffer[9] = 0x6F; //o
//		  I2CMasterBuffer[10] = 0x72;//r
//		  I2CMasterBuffer[11] = 0x6C;//l
//		  I2CMasterBuffer[12] = 0x64;//d
//		  I2CMasterBuffer[13] = 0x3F;//?

		  /* configuration value, no change from default */
		//  I2CCmd = LCD_CONFIG;
		  I2CEngine();
}
