#define __MAIN_C__

#include "type.h"
#include "LPC23xx.h"
#include "uart.h"
#include "led.h"
#include "target.h"

static char Hello[]="\r\nhelloworld";

/*****************************************************************************
**   Main Function  main()
**   UNCHANGED UART TEST CODE
*****************************************************************************/
int main (void)
{
    init_VIC();
    UARTInit(115200);	/* baud rate setting */

	U0IER = IER_THRE | IER_RLS;			/* Disable RBR */
	UARTSend( (BYTE*)Hello, 12 );
	UART0Count = 0;
	U0IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */

    while (1)
    {				/* Loop forever */
	if ( UART0Count != 0 )
	{
		U0IER = IER_THRE | IER_RLS;			/* Disable RBR */
	    UARTSend( UART0Buffer, UART0Count );
	    UART0Count = 0;
	    U0IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */
	}
    }
    return 0;
}

/*int main (void)
{

	PINMODE3 = 0xAAAAAAAA;

	initLED();

	int i;
	i = 0;

	while (1) {
		clearLED();
//		printLED(i++%256);
	}

//
//	FIO1DIR2 |= 0xFF; // Set direction to output
//	FIO1DIR3 |= 0xFF; // Set direction to output
//
//	FIO1SET3 = 0xFF; // set all pins high
//	FIO1SET2 = 0xFF; // set all pins high
//
//	int i;
//	while (1) {
//
//		// turn on LEDs sequentially
//		for (i = 0; i < 8; i++) {
//			if (i < 2)
//				FIO1CLR3 = (1 << i);
//			else
//			  FIO1CLR2 = (1 << i);
//		}
//
//		// turn off LEDs sequentially
//		for (i = 0; i < 8; i++) {
//			if (i < 2)
//				FIO1CLR3 = (1 << i);
//			else
//			  FIO1CLR2 = (1 << i);
//		}
//
//  }
  

	//#define LED0 (1 << 2)	// p1-18 FIO2
	//#define LED1 (1 << 3)	// p1-19 FIO2
	//#define LED2 (1 << 4)	// p1-20 FIO2
	//#define LED3 (1 << 5)	// p1-21 FIO2
	//#define LED4 (1 << 6)	// p1-22 FIO2
	//#define LED5 (1 << 7)	// p1-23 FIO2
	//#define LED6 (1 << 0)	// p1-24 FIO3
	//#define LED7 (1 << 1)	// p1-25 FIO3

	// LEDs
	// bits 19:4 (every 2) of PINSEL3
	//
	//p1[18] - 66
	//p1[19] - 68
	//p1[20] - 70
	//p1[21] - 72
	//p1[22] - 74
	//p1[23] - 76
	//p1[24] - 78
	//p1[25] - 80

  return(0); // prevents compiler warnings
}*/
