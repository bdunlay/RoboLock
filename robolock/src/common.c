#include "type.h"
#include "common.h"
#include "target.h"

void busyWait(DWORD delay_period) {
	volatile int a;
	volatile int b;

	for (a = 0; a < delay_period; a++)
	{
		for (b = 0; b < (3600); b++) ;//for (b = 0; b < (Fpclk / 1000-1); b++) ;
	}
}


inline BYTE hexToChar(BYTE hex)
{
	if (hex < 10)
		return hex + 0x30;
	else
		return hex + 55;
}

inline BYTE atoi(BYTE a)
{
	if (a >= 0x30 && a < 0x3A)
		return a-0x30;
	else
		return (BYTE)0;
}

inline BYTE itoa(BYTE a)
{
	if (a >= 0 && a < 10)
		return a+0x30;
	else
		return (BYTE)0;
}
