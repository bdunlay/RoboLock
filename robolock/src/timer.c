/*****************************************************************************
 *   timer.c:  Timer C file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.09.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"		/* LPC23xx/24xx Peripheral Registers	*/
#include "type.h"
#include "irq.h"
#include "target.h"
#include "type.h"
#include "led.h"
#include "clock-arch.h"
#include "timer.h"


volatile DWORD timer0_counter = 0;
volatile DWORD timer1_counter = 0;

/******************************************************************************
** Function name:		enable_timer
**
** Descriptions:		Enable timer
**
** parameters:			timer number: 0 or 1
** Returned value:		None
** 
******************************************************************************/
void enable_timer( BYTE timer_num )
{
	switch (timer_num)
	{
	case 0:
		T0TCR = 1;
		break;
	case 1:
		T1TCR = 1;
		break;
	case 2:
		T2TCR = 1;
		break;
	case 3:
		T3TCR = 1;
		break;
	default:
		break;
	}
	return;
}

/******************************************************************************
** Function name:		disable_timer
**
** Descriptions:		Disable timer
**
** parameters:			timer number: 0 or 1
** Returned value:		None
** 
******************************************************************************/
void disable_timer( BYTE timer_num )
{
	switch (timer_num)
	{
	case 0:
		T0TCR = 0;
		break;
	case 1:
		T1TCR = 0;
		break;
	case 2:
		T2TCR = 0;
		break;
	case 3:
		T3TCR = 0;
		break;
	default:
		break;
	}
	return;
}

/******************************************************************************
** Function name:		reset_timer
**
** Descriptions:		Reset timer
**
** parameters:			timer number: 0 or 1
** Returned value:		None
** 
******************************************************************************/
void reset_timer( BYTE timerNum )
{
	DWORD regVal;

	switch (timerNum)
	{
	case 0:
		regVal = T0TCR;
		regVal |= 0x02;
		T0TCR = regVal;
		break;
	case 1:
		regVal = T1TCR;
		regVal |= 0x02;
		T1TCR = regVal;
		break;
	case 2:
		regVal = T2TCR;
		regVal |= 0x02;
		T2TCR = regVal;
		break;
	case 3:
		regVal = T3TCR;
		regVal |= 0x02;
		T3TCR = regVal;
		break;
	default:
		break;
	}
  	return;
}

/******************************************************************************
** Function name:		init_timer
**
** Descriptions:		Initialize timer, set timer interval, reset timer,
**						install timer interrupt handler
**
** parameters:			timer number, timer interval in cycles, IRQ handler,
**                      and timer options (defined by timerOptions enum)
** Returned value:		true or false, if the interrupt handler can't be
**						installed, return false.
** 
******************************************************************************/
DWORD init_timer ( BYTE timerNum, DWORD timerInterval, void* irqHandler, BYTE timerOpt )
{
	switch (timerNum)
	{
	case 0:
		T0MR0 = timerInterval;	// set timer interval, in units of clock cycles
		T0MCR = timerOpt;		// set timer to reset/interrupt/both/neither
		return install_irq( TIMER0_INT, (void *)irqHandler, HIGHEST_PRIORITY );
		break;
	case 1:
		T1MR0 = timerInterval;	// set timer interval, in units of clock cycles
		T1MCR = timerOpt;		// set timer to reset/interrupt/both/neither
		return install_irq( TIMER1_INT, (void *)irqHandler, HIGHEST_PRIORITY );
		break;
	case 2:
		PCONP |= (1<<22);
		T2MR0 = timerInterval;	// set timer interval, in units of clock cycles
		T2MCR = timerOpt;		// set timer to reset/interrupt/both/neither
		return install_irq( TIMER2_INT, (void *)irqHandler, HIGHEST_PRIORITY );
		break;
	case 3:
		PCONP |= (1<<23);
		T3MR0 = timerInterval;	// set timer interval, in units of clock cycles
		T3MCR = timerOpt;		// set timer to reset/interrupt/both/neither
		return install_irq( TIMER3_INT, (void *)irqHandler, HIGHEST_PRIORITY );
		break;
	default:
		break;
	}

  return FALSE;
}

void testTimerIRQ(void)
{
	DWORD i = 0;
	timer0_counter = 0;
	T0MR0 = TIME_INTERVAL;
	T0MCR = 3;				/* Interrupt and Reset on MR0 */
	install_irq( TIMER0_INT, (void *)testTimer0Handler, HIGHEST_PRIORITY );
	reset_timer( 0 );
	enable_timer( 0 );
	IENABLE;
	for (i=0; i<TIME_INTERVAL*10; i++) ; // wait for a long time while interrupts do their thing
	disable_timer( 0 );

	timer1_counter = 0;
	T1MR0 = TIME_INTERVAL;
	T1MCR = 3;				/* Interrupt and Reset on MR1 */
	reset_timer( 1 );
	enable_timer( 1 );
	install_irq( TIMER1_INT, (void *)testTimer1Handler, HIGHEST_PRIORITY );
	for (i=0; i<TIME_INTERVAL*10; i++) ; // wait for a long time while interrupts do their thing
	disable_timer( 1 );
}

void testTimer0Handler(void)
{
	T0IR = 1;			/* clear interrupt flag */
	IENABLE;			/* handles nested interrupt */

	printLED(0x01 << timer0_counter++);

	IDISABLE;
	VICVectAddr = 0;	/* Acknowledge Interrupt */
}

void testTimer1Handler(void)
{
	T1IR = 1;			/* clear interrupt flag */
	IENABLE;			/* handles nested interrupt */

	printLED(0x80 >> timer1_counter++);

	IDISABLE;
	VICVectAddr = 0;	/* Acknowledge Interrupt */
}


/******************************************************************************
**                            End Of File
******************************************************************************/
