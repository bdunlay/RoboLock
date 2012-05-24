#include "LPC23xx.h"
#include "uart3.h"
#include "keypad.h"
#include "type.h"
#include "target.h"
#include "common.h"
#include "led.h"
#include "LCD.h"

void testKeypad(void) {
	IENABLE;
	while (1) {
		printLED(keypadValue);
		busyWait(50);
	}
}

void keypadVerify(void) {
	IENABLE;
	keypadCount = 0;
	int last = 0;
	lcdClear();
	char displayCode[16] = "Enter Code:     ";
	lcdDisplay(displayCode, "                ");
	while (keypadCount < 4) {
		if (keypadCount >= 1) {
			displayCode[10 + keypadCount] = keypadValue;
			if (keypadCount >= 2)
				displayCode[10 + keypadCount - 1] = '*';
		}
		if (last < keypadCount) {
			last++;//last = keypadCount;
			lcdDisplay(displayCode, "                ");
			//printLED(keypadCount);
		}
	}
	keypadCount = 0;
}
