#define __MAIN_C__

#include "type.h"
#include "LPC23xx.h"
#include "target.h"
#include "common.h"
#include "led.h"
#include "dip.h"
#include "LCD.h"
#include "keypad.h"
#include "cameraB.h"
#include "ex_sdram.h"
#include "timer.h"
#include "uart.h"
#include "adc.h"
#include "uart.h"
#include "uart2.h"
#include "uart3.h"
#include "ethernet.h"
#include "strike.h"
#include "robolock.h"
#include "button.h"

/*****************************************************************************
 *    Main Function  main()													 *
 *****************************************************************************/

int main(void) {

	IENABLE;
	TargetResetInit();
	initLED();
	RTCInit();
	printLED(0x01);
	initLCD();
	init_dip();
	init_button();
	UARTInit(9600);
	printLED(0x03);
	UART2Init(38400);
	printLED(0x07);
	UART3Init(9600);
	printLED(0x0F);
	ADCInit();
	printLED(0x1F);
	PCONP &= ~(1<<11); // turn off EMC :(    //SDRAMInit();
	strikeInit();
	printLED(0x3F);
	init_robolock();
	printLED(0x7F);
	init_network();
	printLED(0xFF);
	IENABLE;
	busyWait(500);
	lcdInit();  //this needs inturrupts
	printLED(0x00);


//	while (1) {
//		robolock();
//	}
		while(1) {
		switch (read_dip()) {

		case 0:

			break;

			/* SUBSYSTEM TESTS */

		case 1:
			testLED();
			break;

		case 2:
			//testLCD();
			break;

		case 3:
			testUART();
			break;

		case 4:
			testTimerIRQ();
			break;

		case 5:
			testStrike();
			break;

		case 6:
			testADC();
			break;

		case 7:
			testCamera();
			break;

		case 8:
			testSDRAM();
			break;

		case 9:
			testSDRAM_simple();
			break;

		case 10:
			testKeypad();
			break;

		case 11:
			robolock();
			break;

		case 12:
			break;

		case 13:
			break;

		case 14:
			break;

		case 15:
			break;

		default:
			break;
		}

		return 0;
	}

}

