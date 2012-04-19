#define __MAIN_C__

#include "type.h"
#include "LPC23xx.h"
//#include "target.h"
//#include "timer.h"
//#include "irq.h"

//#include "typedefs.h"

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

void initLED(void);
void clearLED(void);
void printLED(unsigned int);

// set LED GPIOs as output
void initLED(void) {
	PINSEL2 = 0x00;
	PINSEL3 = 0x00; // (Re)Set to GPIO

	FIO1DIR2 = 0xFC;
	FIO1DIR3 = 0x03;
	clearLED();

}

// set all GPIOs high
void clearLED(void) {
	FIO1SET2 = 0xFC;
	FIO1SET3 = 0x03;
}


void printLED(unsigned int val) {

	clearLED();

	int i;

	for (i = 0; i < 8; i++) {
		if (i < 6)
			FIO2CLR |= ((val & (0x1 << i)) << 2);
		else
			FIO2CLR |= ((val & (0x1 << i)) >> 6);
	}
}



static void prvSetupHardware( void )
{

	MAMCR  = 0x00;  /* Off                                        */
	MAMTIM = 0x04;  /* 4 Fetch cycles, gives better result than 3 */
	MAMCR  = 0x02;  /* Fully enabled                              */

	MEMMAP = 0x01;  /* Uses interrupts vectors in flash */

	PCONP |= (1 << 31);	
	(*(volatile unsigned int *)(0xFFE0C008)) = 0x1;

	/* Start main oscillator */
	SCS |= (1<<5);

	while ((SCS & (1<<6)) == 0)
		;

	/* Select PLL as source */
	CLKSRCSEL = 0x01;

	/* Start PLL */
	PLLCFG = 11 | (0 << 16);
	PLLFEED   = 0xAA;
	PLLFEED   = 0x55;

	PLLCON    = 0x01;
	PLLFEED   = 0xAA;
	PLLFEED   = 0x55;

	/* Wait until locked */
	while ((PLLSTAT & (1<<26)) == 0)
		;

	/* Wait until M and N are correct */
	while ((PLLSTAT & 0xFF7FFF) != 0x0000000B)
		;

	/* Setup CPU clock divider */
	//CCLKCFG = 3;
	CCLKCFG = 4; //57600000

	/* Setup USB clock divider */
	USBCLKCFG = 5; //48Mhz

	/* Setup Peripheral clock */
	PCLKSEL0 = 0;
	PCLKSEL1 = 0;

	/* Switch to PLL clock */
	PLLCON   = 0x03;
	PLLFEED  = 0xAA;
	PLLFEED  = 0x55;

	/* Make sure we are connected to PLL */
	while ((PLLSTAT & (1<<25))==0)
		;

	SCS |= (1<<0); //fast mode for port 0 and 1


}
/*-----------------------------------------------------------*/



int main (void)
{


	prvSetupHardware();

   // init_VIC();
   // init_timer();

    
   // GPIOInit( REGULAR_PORT );		/* Initialize GPIO pins, default is REGULAR_PORT  */

    
   // enable_timer( 0 );			/* Initialize timer for GPIO toggling timing */

   


	initLED();

	int i;
	i = 0;

	while (1) {
		printLED(i++%256);
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
  
  return(0); // prevents compiler warnings
}


