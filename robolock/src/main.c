#define __MAIN_C__

#include "type.h"
#include "LPC23xx.h"
//#include "uart.h"
#include "led.h"
#include "target.h"


void init_dip(void);
unsigned short read_dip(void);
void init_robolock(void);
void robolock(void);

// static char Hello[]="\r\nhelloworld";


/*****************************************************************************
 *    Main Function  main()													 *
 *****************************************************************************/

int main (void)
{
	// put all initializations in this function
	init_robolock();

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
				break;

			case 3:
				break;

			case 4:
				break;

			case 5:
				break;

			case 6:
				break;

			case 7:
				break;

			case 8:
				break;

			case 9:
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



//	UARTInit(115200);	/* baud rate setting */
//
//	U0IER = IER_THRE | IER_RLS;			/* Disable RBR */
//	UARTSend( (BYTE*)Hello, 12 );
//	UART0Count = 0;
//	U0IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */
//
//    while (1)
//    {				/* Loop forever */
//	if ( UART0Count != 0 )
//	{
//		U0IER = IER_THRE | IER_RLS;			/* Disable RBR */
//	    UARTSend( UART0Buffer, UART0Count );
//	    UART0Count = 0;
//	    U0IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */
//	}
//    }
    return 0;
}


/*****************************************************************************
 *    Will factor these out later
 *****************************************************************************/

void init_dip() {
	FIO2DIR0 = 0x00;
	FIO2CLR0 = 0x1E;
}

unsigned short read_dip() {
	return (FIO2PIN0>>1) & 0xF;
}

void init_robolock() {

	TargetResetInit(); // PLL
	initLED();
	init_dip();
}

// main program
void robolock() {

}
