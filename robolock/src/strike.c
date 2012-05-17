/* RoboLock
 * strike.C
 *
 * Source file for Door strike control on the RoboLock board
 *
 * v1.0 - 4/22/2012
 */

#include "LPC23xx.h"
#include "strike.h"
#include "type.h"
#include "target.h"
#include "common.h"
#include "led.h"
void strikeInit(void) {

	PINSEL4 &= ~0x3;  // set p2[0] to 0
	PINMODE4 &= ~0x3;
	PINMODE4 |= 0x2;  // no pullup
	FIO2DIR0 &= ~0x3;
	FIO2DIR0 |= 0x1;

	//strikeOpen();

}

void strikeClose(){
	FIO2SET0 = 0x1;
	printLED(0x0F);
}

void strikeOpen(){
	FIO2CLR0= 0x1;
	printLED(0xF0);
}

void testStrike(){
	busyWait(400);
	strikeClose();
	busyWait(400);
	strikeOpen();

}
