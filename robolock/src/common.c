#include "type.h"
#include "common.h"
#include "target.h"


void busyWait(DWORD delay_period) {
	volatile int a;
	volatile int b;

	for (a = 0; a < delay_period; a++)
	{
		for (b = 0; b < (Fpclk / 1000-1); b++) ;
	}
}

BYTE hexToChar(BYTE hex)
{
	if (hex < 10)
		return hex + 0x30;
	else
		return hex + 55;
}
