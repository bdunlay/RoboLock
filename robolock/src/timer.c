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
#include "typedefs.h"
#include "irq.h"
#include "target.h"
#include "type.h"
#include "led.h"
#include "timer.h"

volatile DWORD timer0_counter = 0;
volatile DWORD timer1_counter = 0;

/*****************************************************************************
** Function name:		delayMs
**
** Descriptions:		Start the timer delay in milo seconds
**						until elapsed
**
** parameters:			timer number, Delay value in milo second			 
** 						
** Returned value:		None
** 
*****************************************************************************/
void delayMs(BYTE timer_num, DWORD delayInMs)
{
  if ( timer_num == 0 )
  {
	/*
	* setup timer #0 for delay
	*/
	T0TCR = 0x02;		/* reset timer */
	T0PR  = 0x00;		/* set prescaler to zero */
	T0MR0 = delayInMs * (Fpclk / 1000-1);
	T0IR  = 0xff;		/* reset all interrrupts */
	T0MCR = 0x04;		/* stop timer on match */
	T0TCR = 0x01;		/* start timer */
  
	/* wait until delay time has elapsed */
	while (T0TCR & 0x01);
  }
  else if ( timer_num == 1 )
  {
	/*
	* setup timer #1 for delay
	*/
	T1TCR = 0x02;		/* reset timer */
	T1PR  = 0x00;		/* set prescaler to zero */
	T1MR0 = delayInMs * (Fpclk / 1000-1);
	T1IR  = 0xff;		/* reset all interrrupts */
	T1MCR = 0x04;		/* stop timer on match */
	T1TCR = 0x01;		/* start timer */
  
	/* wait until delay time has elapsed */
	while (T1TCR & 0x01);
  }
  return;
}

#if !FIQ
/******************************************************************************
** Function name:		Timer0Handler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**						executes each 10ms @ 60 MHz CPU Clock
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void Timer0Handler (void)
{  
  T0IR = 1;			/* clear interrupt flag */
  IENABLE;			/* handles nested interrupt */

  timer0_counter++;

  IDISABLE;
  VICVectAddr = 0;	/* Acknowledge Interrupt */
}

#else
/******************************************************************************
** Function name:		Timer0FIQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**						executes each 10ms @ 60 MHz CPU Clock
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void Timer0FIQHandler(void) 
{  
  T0IR = 1;			/* clear interrupt flag */
  timer0_counter++;
//  VICVectAddr = 0;	/* Acknowledge Interrupt */
}
#endif

#if !FIQ
/******************************************************************************
** Function name:		Timer1Handler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**						executes each 10ms @ 60 MHz CPU Clock
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void Timer1Handler (void)
{  
  T1IR = 1;			/* clear interrupt flag */
  IENABLE;			/* handles nested interrupt */

  timer1_counter++;

  IDISABLE;
  VICVectAddr = 0;	/* Acknowledge Interrupt */
}

#else
/******************************************************************************
** Function name:		Timer1Handler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**						executes each 10ms @ 60 MHz CPU Clock
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void Timer1FIQHandler (void)
{  
  T1IR = 1;			/* clear interrupt flag */
  timer1_counter++;
//  VICVectAddr = 0;	/* Acknowledge Interrupt */
}
#endif

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
  if ( timer_num == 0 )
  {
	T0TCR = 1;
  }
  else
  {
	T1TCR = 1;
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
  if ( timer_num == 0 )
  {
	T0TCR = 0;
  }
  else
  {
	T1TCR = 0;
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
void reset_timer( BYTE timer_num )
{
  DWORD regVal;

  if ( timer_num == 0 )
  {
	regVal = T0TCR;
	regVal |= 0x02;
	T0TCR = regVal;
  }
  else
  {
	regVal = T1TCR;
	regVal |= 0x02;
	T1TCR = regVal;
  }
  return;
}

/******************************************************************************
** Function name:		init_timer
**
** Descriptions:		Initialize timer, set timer interval, reset timer,
**						install timer interrupt handler
**
** parameters:			timer number and timer interval
** Returned value:		true or false, if the interrupt handler can't be
**						installed, return false.
** 
******************************************************************************/
DWORD init_timer ( BYTE timer_num, DWORD TimerInterval ) 
{
  if ( timer_num == 0 )
  {
	timer0_counter = 0;
	T0MR0 = TimerInterval;
	T0MCR = 3;				/* Interrupt and Reset on MR0 */
#if FIQ
	/* FIQ is always installed. */
	VICIntSelect |= (0x1<<4);
	VICIntEnable = (0x1<<4);
	return (TRUE);
#else
	if ( install_irq( TIMER0_INT, (void *)Timer0Handler, HIGHEST_PRIORITY ) == FALSE )
	{
	  return (FALSE);
	}  
	else
	{
	  return (TRUE);
	}
#endif
  }
  else if ( timer_num == 1 )
  {
	timer1_counter = 0;
	T1MR0 = TimerInterval;
	T1MCR = 3;				/* Interrupt and Reset on MR1 */
#if FIQ
	VICIntSelect |= (0x1<<5);
	VICIntEnable = (0x1<<5);
	return (TRUE);
#else
	if ( install_irq( TIMER1_INT, (void *)Timer1Handler, HIGHEST_PRIORITY ) == FALSE )
	{
	  return (FALSE);
	}  
	else
	{
	  return (TRUE);
	}
#endif
  }
  return (FALSE);
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

	printLED(0x80 << timer1_counter++);

	IDISABLE;
	VICVectAddr = 0;	/* Acknowledge Interrupt */
}

/******************************************************************************
**                            End Of File
******************************************************************************/
