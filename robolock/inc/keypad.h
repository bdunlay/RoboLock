#ifndef __KEYPAD_H
#define __KEYPAD_H
#include "uart3.h"

void testKeypad(void);
BYTE keypadVerify(void);

volatile BYTE keypadValue;
volatile int keypadCount;


#endif
