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

//volatile BYTE I2CMasterBuffer[BUFSIZE];
//BYTE buff;

void initLCD(void) {

	I2CInit(I2CMASTER); // start master
	FIO1DIR3 |= 0x04;
	PINSEL1 &= ~0x00008000;
	FIO1SET3 |= 0x04;

}

void lcdBacklight(void) {

	FIO1CLR3 |= 0x04;

}

void lcdBacklightOff(void) {
	FIO1SET3 |= 0x04;

}
void lcdInit() {
	volatile int i = 0;
	for (i = 0; i < BUFSIZE; i++) /* clear buffer */
	{
		I2CMasterBuffer[i] = 0;
	}
	I2CWriteLength = 2;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x00;
	I2CMasterBuffer[2] = 0x38; //function select is=0
	//	I2CMasterBuffer[3] = 0x39; //select is=1
	//	I2CMasterBuffer[4] = 0x14; //right direction
	//	I2CMasterBuffer[5] = 0x72; //78
	//	I2CMasterBuffer[6] = 0x5E;
	//	I2CMasterBuffer[7] = 0x6D;
	//	I2CMasterBuffer[8] = 0x0C; //0x0F
	//	I2CMasterBuffer[9] = 0x01;
	//	I2CMasterBuffer[10] = 0x06;
	/* configuration value, no change from default */
	//I2CCmd = LCD_CONFIG;
	I2CEngine();
	for (i = 0; i < 5000; i++)
		;
	////
	for (i = 0; i < BUFSIZE; i++) /* clear buffer */
	{
		I2CMasterBuffer[i] = 0;
	}
	I2CWriteLength = 2;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x00;
	I2CMasterBuffer[2] = 0x39;
	I2CEngine();
	for (i = 0; i < 5000; i++)
		;
	////
	for (i = 0; i < BUFSIZE; i++) /* clear buffer */
	{
		I2CMasterBuffer[i] = 0;
	}
	I2CWriteLength = 2;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x00;
	I2CMasterBuffer[2] = 0x14;
	I2CEngine();
	for (i = 0; i < 2000; i++)
		;
	////
	for (i = 0; i < BUFSIZE; i++) /* clear buffer */
	{
		I2CMasterBuffer[i] = 0;
	}
	I2CWriteLength = 2;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x00;
	I2CMasterBuffer[2] = 0x72;
	I2CEngine();
	for (i = 0; i < 2000; i++)
		;
	////
	for (i = 0; i < BUFSIZE; i++) /* clear buffer */
	{
		I2CMasterBuffer[i] = 0;
	}
	I2CWriteLength = 2;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x00;
	I2CMasterBuffer[2] = 0x5E;
	I2CEngine();
	for (i = 0; i < 2000; i++)
		;
	////
	for (i = 0; i < BUFSIZE; i++) /* clear buffer */
	{
		I2CMasterBuffer[i] = 0;
	}
	I2CWriteLength = 2;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x00;
	I2CMasterBuffer[2] = 0x6D;
	I2CEngine();
	for (i = 0; i < 2000; i++)
		;
	////
	for (i = 0; i < BUFSIZE; i++) /* clear buffer */
	{
		I2CMasterBuffer[i] = 0;
	}
	I2CWriteLength = 2;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x00;
	I2CMasterBuffer[2] = 0x0C;
	I2CEngine();
	for (i = 0; i < 2000; i++)
		;
	////
	for (i = 0; i < BUFSIZE; i++) /* clear buffer */
	{
		I2CMasterBuffer[i] = 0;
	}
	I2CWriteLength = 2;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x00;
	I2CMasterBuffer[2] = 0x01;
	I2CEngine();
	for (i = 0; i < 2000; i++)
		;
	////
	for (i = 0; i < BUFSIZE; i++) /* clear buffer */
	{
		I2CMasterBuffer[i] = 0;
	}
	I2CWriteLength = 2;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x00;
	I2CMasterBuffer[2] = 0x06;
	I2CEngine();
	for (i = 0; i < 10000; i++)
		;
}

void lcdClear() {
	volatile int i = 0;
	for (i = 0; i < BUFSIZE; i++) /* clear buffer */
	{
		I2CMasterBuffer[i] = 0;
	}
	I2CWriteLength = 2;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x00;
	I2CMasterBuffer[2] = 0x01;
	I2CEngine();
	for (i = 0; i < 10000; i++)
		;
}

void LCDWrite(char* buffer) {
	volatile int i = 0;
	volatile int j = 0;
	for (i = 0; i < 16; i++) {
		for (j = 0; j < BUFSIZE; j++) /* clear buffer */
		{
			I2CMasterBuffer[j] = 0;
		}

		I2CWriteLength = 2;
		I2CReadLength = 0;
		I2CMasterBuffer[0] = LCD_ADDR;
		I2CMasterBuffer[1] = 0x40;
		I2CMasterBuffer[2] = buffer[i];
		I2CEngine();
		for (j = 0; j < 10000; j++)
			;
	}

}

void LCDLine1() {
	volatile int i = 0;
	for (i = 0; i < BUFSIZE; i++) /* clear buffer */
	{
		I2CMasterBuffer[i] = 0;
	}
	I2CWriteLength = 2;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x00;
	I2CMasterBuffer[2] = 0x80;
	I2CEngine();
	for (i = 0; i < 2000; i++)
		;
}

void LCDLine2() {
	volatile int i = 0;

	for (i = 0; i < BUFSIZE; i++) /* clear buffer */
	{
		I2CMasterBuffer[i] = 0;
	}
	I2CWriteLength = 2;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = LCD_ADDR;
	I2CMasterBuffer[1] = 0x00;
	I2CMasterBuffer[2] = 0xC0;
	I2CEngine();
	for (i = 0; i < 10000; i++)
		;
}

void lcdDisplay(char* line1, char* line2) {
	lcdClear();
	LCDLine1();
	LCDWrite(line1);
	LCDLine2();
	LCDWrite(line2);
	//busyWait(10);
}

void printLCD(BYTE* str)
{
	BYTE a[16];
	BYTE i;
	BYTE buffIdx;
	BYTE wordLen;
	BYTE spaceLeft;
	BYTE whichLine = 1;
	while (*str != '\0')
	{
		spaceLeft = 16;
		buffIdx = 0;
		while (spaceLeft > 0)
		{
			// find next word length
			wordLen = 0;
			while (!(str[wordLen] == ' ' || str[wordLen] == '\0'))
				wordLen++;

			if (wordLen <= spaceLeft)	// if the word will fit
			{
				for (i=0; i<wordLen; i++)
				{
					a[buffIdx+i] = str[i];	// copy into buffer
					--spaceLeft;
				}
				buffIdx += wordLen;
				str += wordLen;				// move up the str pointer to the next word
			}
			else if (wordLen > 16)		// if the word is larger than the screen
			{
				for (i=0; spaceLeft > 0; i++)
				{
					a[buffIdx+i] = str[i];	// only copy what will fit
					--spaceLeft;
				}
				str += wordLen;				// move up the str pointer to the next word
			}
			else						// if the word will fit on the next line
			{
				while (spaceLeft > 0)	// fill the rest with spaces
				{
					a[16-spaceLeft] = ' ';
					--spaceLeft;
				}
			}

			if (*str == '\0')			// if we're at the end of the string
			{
				while (spaceLeft > 0)	// fill the rest with spaces
				{
					a[16-spaceLeft] = ' ';
					--spaceLeft;
				}
			}
			else
			{
				if (spaceLeft > 0)		// add the ' ' char if there's room for the space
				{
					a[buffIdx++] = ' ';
					--spaceLeft;
				}
				++str;					// increment the str pointer past the space
			}
		}

		// print to LCD
		if (whichLine == 1)
		{
			lcdClear();
			LCDLine1();
			LCDWrite(a);
			whichLine = 2;
			if (*str == '\0')
				busyWait(5000);
		}
		else
		{
			LCDLine2();
			LCDWrite(a);
			whichLine = 1;
			busyWait(5000);
		}
	}
}

void testLCD()
{
	lcdBacklight();
	printLCD("This is a really long message. It should be broken up correctly. W00t");
	while (1);
}
