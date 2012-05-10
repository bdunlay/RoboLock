#define __MAIN_C__

#include "type.h"
#include "LPC23xx.h"
#include "target.h"
#include "common.h"
#include "led.h"
#include "dip.h"
#include "LCD.h"
#include "robolock.h"
#include "ex_sdram.h"
#include "timer.h"
#include "uart.h"
#include "adc.h"
#include "uart.h"

/*****************************************************************************
 *    Main Function  main()													 *
 *****************************************************************************/

int main (void)
{

	TargetResetInit();
	initLED();
	initLCD();
	init_dip();

	UARTInit(0, 9600);

	//UARTInit();
	ADCInit();
	SDRAMInit();

	while(1) {

		switch(read_dip()) {



			/* MAIN PROGRAM */

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
	}


    return 0;
}


