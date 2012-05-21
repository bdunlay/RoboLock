/*****************************************************************************
 *   timer.h:  Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.13  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __TIMER_H 
#define __TIMER_H

#include "type.h"
#include "irq.h"
	
#define TIME_INTERVAL	(180000-1)

enum timerOptions
{
	TIMEROPT_NONE = 0,
	TIMEROPT_INT = 1,
	TIMEROPT_RST = 2,
	TIMEROPT_INT_RST = 3
};

extern DWORD init_timer( BYTE timer_num, DWORD timerInterval, void* irqHandler, BYTE timerOpt );
extern void enable_timer( BYTE timer_num );
extern void disable_timer( BYTE timer_num );
extern void reset_timer( BYTE timer_num );
void testTimerIRQ(void);
extern void testTimer0Handler(void) __irq;
extern void testTimer1Handler(void) __irq;

#endif /* end __TIMER_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
