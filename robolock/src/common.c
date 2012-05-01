#include "common.h"
#define DELAY_PERIOD 100
void delay() {
	volatile int a;
	volatile int b;
	volatile int c;

	for (a = 0; a < DELAY_PERIOD; a++) {
		for (b = 0; b < 32767; b++) {
		}
	}
}

