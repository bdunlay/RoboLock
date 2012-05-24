#include "LPC23xx.h"
#include "uart3.h"
#include "keypad.h"
#include "type.h"
#include "target.h"
#include "common.h"
#include "led.h"
#include "LCD.h"


void testKeypad(void){
	IENABLE;
	while(1){
		printLED(keypadValue);
		busyWait(50);
	}
}

void keypadVerify(void){
	IENABLE;
	keypadCount=-1;
	//lcdInit();
	lcdClear();
	char displayCode[16] = "                ";
	while(keypadCount<4){
		busyWait(40);
		//if(keypadCount == 0)	lcdBacklight();
		if(keypadCount>=0)
			{
			displayCode[keypadCount]=keypadValue;
			if(keypadCount>0)displayCode[keypadCount-1]='*';
			}

		lcdDisplay(displayCode,"                ");
	}
	//keypadCount = 0;
	busyWait(40);
}
