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
#include "robolock.h"

extern volatile DWORD I2CCount;
extern volatile BYTE I2CMasterBuffer[BUFSIZE];
extern volatile DWORD I2CCmd, I2CMasterState;
extern volatile DWORD I2CReadLength, I2CWriteLength;

volatile BYTE I2CMasterBuffer[BUFSIZE];
BYTE buff;

void initLCD(void) {

	I2CInit(I2CMASTER);  // start master
	FIO1DIR3 |= 0x04;
	PINSEL1 &= ~0x00008000;
	FIO1SET3 |= 0x04;

}

void lcdBacklight(void){

	FIO1CLR3 |= 0x04;


}

void lcdBacklightOff(void){
	FIO1SET3 |= 0x04;

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
			//  busyWait(200);
}


void testLCD() {
		//volatile int i;
		init_robolock();

		lcdBacklight();

		lcdInit();
		//for(i=0;i<600;i++);
		lcdClear();
		//for(i=0;i<600;i++);
		LCDWrite("Hello There");
		//for(i=0;i<600;i++);
		LCDLine2();
		//for(i=0;i<600;i++);
		LCDWrite("Test Program");
		busyWait(1000);

}

void lcdInit(){
	volatile int i=0;
	for(i=0;i<600;i++);
		  for ( i = 0; i < BUFSIZE; i++ )	/* clear buffer */
		  {
			I2CMasterBuffer[i] = 0;
		  }
		  I2CWriteLength = 10;
		  I2CReadLength = 0;
		  I2CMasterBuffer[0] = LCD_ADDR;
		  I2CMasterBuffer[1] = 0x00;
		  I2CMasterBuffer[2] = 0x38; //function select is=0
		  I2CMasterBuffer[3] = 0x39; //select is=1
		  I2CMasterBuffer[4] = 0x14; //right direction
		  I2CMasterBuffer[5] = 0x72;  //78
		  I2CMasterBuffer[6] = 0x5E;
		  I2CMasterBuffer[7] = 0x6D;
		  I2CMasterBuffer[8] = 0x0C;  //0x0F
		  I2CMasterBuffer[9] = 0x01;
		  I2CMasterBuffer[10] = 0x06;


		  /* configuration value, no change from default */
		  I2CCmd = LCD_CONFIG;
		  I2CEngine();
}

void lcdClear(){
	volatile int i=0;
	for(i=0;i<600;i++);
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
		  for(i=0;i<600;i++);
}

void LCDWrite(char* buffer){
	volatile int i = 0;
	for(i=0;i<600;i++);
	I2CWriteLength = 17;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x40;
	for(i=0;i<16;i++){
		I2CMasterBuffer[i+2] = buffer[i];
	}
	for(i=0;i<600;i++);
	 I2CEngine();

}

void LCDLine1(){
	volatile int i=0;
	for(i=0;i<600;i++);
			  for ( i = 0; i < BUFSIZE; i++ )	/* clear buffer */
			  {
				I2CMasterBuffer[i] = 0;
			  }
			  I2CWriteLength = 10;
			  I2CReadLength = 0;
			  I2CMasterBuffer[0] = LCD_ADDR;
			  I2CMasterBuffer[1] = 0x00;
			  I2CMasterBuffer[5] = 0x80;

			  I2CEngine();
}

void LCDLine2(){
	volatile int i=0;
	for(i=0;i<600;i++);
			  for ( i = 0; i < BUFSIZE; i++ )	/* clear buffer */
			  {
				I2CMasterBuffer[i] = 0;
			  }
			  I2CWriteLength = 10;
			  I2CReadLength = 0;
			  I2CMasterBuffer[0] = LCD_ADDR;
			  I2CMasterBuffer[1] = 0x00;
			  I2CMasterBuffer[5] = 0xC0;

			  I2CEngine();
}

void lcdDisplay(char* line1,char* line2){

	lcdClear();
	LCDWrite(line1);
	LCDLine2();
	LCDWrite(line2);
}
