#define __MAIN_C__

#include <stdio.h> 
#include <stdlib.h> 
#include "LPC23xx.h"
#include "typedefs.h"

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

// Hello World

int main (void)
{

	PINSEL3 = 0x00; // (Re)Set to GPIO

	FIO1DIR2 |= 0xFF; // Set direction to output
	FIO1DIR3 |= 0xFF; // Set direction to output

	FIO1SET3 = 0xFF; // set all pins high
	FIO1SET2 = 0xFF; // set all pins high

	int i;
	while (1)
	{

		// turn on LEDs sequentially
		for (i = 0; i < 8; i++) {
			if (i < 2)
				FIO1CLR3 = (1 << i);
			else
			  FIO1CLR2 = (1 << i);
		}

		// turn off LEDs sequentially
		for (i = 0; i < 8; i++) {
			if (i < 2)
				FIO1CLR3 = (1 << i);
			else
			  FIO1CLR2 = (1 << i);
		}

  }
  
  return(0); // prevents compiler warnings
}
