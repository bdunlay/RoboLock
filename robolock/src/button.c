/*
 * button.c
 *
 *  Created on: May 26, 2012
 *
 */

#include "lpc23xx.h"
#include "irq.h"
#include "type.h"
#include "button.h"

void init_button(void)
{
	PINSEL4  &= 0xFFCFFFFF;
	PINSEL4  |= 0x00100000;	// set to external interrupt
	PINMODE4 &= 0xFFCFFFFF;	// set to pull-up resistor

	IO2_INT_EN_F = 0x200;	// falling edge
	EXTMODE = 0x01;			// EINT0 is edge-triggered
	EXTPOLAR = 0;			// active low

	buttonPressed = FALSE;

	install_irq(EINT0_INT,(void *)EINT0_Handler, HIGHEST_PRIORITY+3 );
}

void EINT0_Handler(void)
{
	EXTINT = 0x01;			// clear interrupt
	IENABLE;

	buttonPressed = TRUE;

	IDISABLE;
	VICVectAddr = 0;
}
