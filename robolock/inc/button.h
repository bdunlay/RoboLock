/*
 * button.h
 *
 *  Created on: May 26, 2012
 *
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "irq.h"
#include "type.h"

void init_button(void);
void EINT0_Handler(void) __irq;

volatile BYTE buttonPressed;

#endif /* BUTTON_H_ */
