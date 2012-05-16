#include "type.h"
#include "common.h"


void busyWait(unsigned int delay_period) {
	volatile int a;
	volatile int b;

	for (a = 0; a < delay_period; a++) {
		for (b = 0; b < 32767; b++) {
		}
	}
}

BYTE hexToChar(BYTE hex)
{
	if (hex < 10)
		return hex + 0x30;
	else
		return hex + 0x41;
}
