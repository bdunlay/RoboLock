#define __MAIN_C__

#include "type.h"
#include "LPC23xx.h"
#include "target.h"
#include "common.h"
#include "led.h"
#include "dip.h"
#include "LCD.h"
#include "keypad.h"
#include "ex_sdram.h"
#include "timer.h"
#include "uart.h"
#include "adc.h"
#include "uart.h"
#include "uart3.h"
#include "ethernet.h"
#include "strike.h"
#include "robolock.h"
#define UIP_ENABLED 1

#if UIP_ENABLED

//#include "uip_timer.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "clock-arch.h"
#include "clock.h"
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#endif

/*****************************************************************************
 *    Main Function  main()													 *
 *****************************************************************************/

int main(void) {

	TargetResetInit();
	initLED();
	initLCD();
	init_dip();

	UARTInit(9600);
	UART3Init(9600);
	ADCInit();
	SDRAMInit();
	strikeInit();
	init_robolock();


	while (1) {

		switch (read_dip()) {

		case 0:

			robolock();

			break;

			/* SUBSYSTEM TESTS */

		case 1:
			testLED();
			break;

		case 2:
			testLCD();
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

