#include "LPC23xx.h"
#include "uart3.h"
#include "keypad.h"
#include "type.h"
#include "target.h"
#include "common.h"
#include "led.h"


void testKeypad(void){
	IENABLE;
	while(1){
		printLED(dummy3);
		busyWait(50);
	}
}
