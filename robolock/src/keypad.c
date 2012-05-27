#include "LPC23xx.h"
#include "uart3.h"
#include "keypad.h"
#include "type.h"
#include "target.h"
#include "common.h"
#include "led.h"
#include "LCD.h"
#include "code.h"
#include "uart.h"
#include "robolock.h"
#include "timer.h"

void testKeypad(void) {
	IENABLE;
	init_timer(3, Fpclk * 2, (void*) periodic_network, TIMEROPT_INT_RST);
	reset_timer(3);

	enable_timer(3);
	while (1) {
		printLED(keypadValue);
		busyWait(50);
	}
}

BYTE keypadVerify(void) {
	IENABLE;
	BYTE code[CODE_LEN];

	keypadCount = 0;
	int last = 0;
	lcdClear();
	char displayCode[16] = "Enter Code:     ";
	lcdDisplay(displayCode, "                ");
	while (keypadCount < CODE_LEN) {
		if (keypadCount >= 1) {
			displayCode[10 + keypadCount] = keypadValue;
			code[keypadCount-1] = keypadValue;
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
//	UARTSendChar(code[0]);
//	UARTSendChar(code[1]);
//	UARTSendChar(code[2]);
//	UARTSendChar(code[3]);
//	UARTSendChar(' ');
	return codeMatches((BYTE*)code);
}
