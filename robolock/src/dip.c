/* RoboLock
 * dip.c
 *
 * Source file for GPIO control, configuration and usage of
 * the dip switch on the RoboLock board
 *
 * v1.0 - 4/22/2012
 */

#include "LPC23xx.h"
#include "dip.h"

/* initializes GPIOs on the dip switch */
void init_dip() {
	FIO2DIR0 = 0x00;
	FIO2CLR0 = 0x1E;
}

/* reads and returns the value of the dip switch */
unsigned short read_dip() {
	return (FIO2PIN0>>1) & 0xF;
}
