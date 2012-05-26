/*****************************************************************************
 *   adc.c:  ADC module file for NXP LPC23xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.08.15  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"                        /* LPC23xx definitions */
#include "type.h"
#include "irq.h"
#include "target.h"
#include "adc.h"

volatile DWORD ADC0IntDone = 0;

#if ADC_INTERRUPT_FLAG
/******************************************************************************
** Function name:		ADC0Handler
**
** Descriptions:		ADC0 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void ADC0Handler (void)
{
  DWORD regVal;
  
  regVal = AD0STAT;		/* Read ADC will clear the interrupt */
  if ( regVal & 0x0000FF00 )	/* check OVERRUN error first */
  {
	regVal = (regVal & 0x0000FF00) >> 0x08;
	/* if overrun, just read ADDR to clear */
	/* regVal variable has been reused. */
	if (regVal & 0x10)
		regVal = AD0DR4;
  }
    
  if ( regVal & ADC_ADINT )
  {
	if (regVal & 0x10) // if AD0[4] is done
		ADC0Value = ( AD0DR4 >> 6 ) & 0x3FF;
  }
  AD0CR &= 0xF8FFFFFF;	/* stop ADC now */
  ADC0IntDone = 1;
  VICVectAddr = 0;		/* Acknowledge Interrupt */
}
#endif

/*****************************************************************************
** Function name:		ADCInit
**
** Descriptions:		initialize ADC channel
**
** parameters:			ADC clock rate
** Returned value:		true or false
** 
*****************************************************************************/
DWORD ADCInit( void )
{
  /* Enable CLOCK into ADC controller */
  PCONP |= (1 << 12);

  /* set pin 42, A0[4] to ADC */
  PINSEL3 |= 0x30000000;	/* A0[4], function 11 */

  AD0CR = ( 0x1 << 4 ) | 	/* SEL=1,select channel 4 on ADC0 */
		( 3 << 8 ) |        /* CLKDIV = Fpclk / (3+1) , should be <=4.5Mhz */
		( 0 << 16 ) | 		/* BURST = 0, no BURST, software controlled */
		( 0 << 17 ) |  		/* CLKS = 0, 11 clocks/10 bits */
		( 1 << 21 ) |  		/* PDN = 1, normal operation */
		( 0 << 22 ) |  		/* TEST1:0 = 00 */
		( 0 << 24 ) |  		/* START = 0 don't do A/D conversion yet */
		( 0 << 27 );		/* EDGE = 0 (CAP/MAT singal falling,trigger A/D conversion) */ 

  ADC0Value = 0;

  /* If POLLING, no need to do the following */
#if ADC_INTERRUPT_FLAG
  AD0INTEN = 0x110;		/* Enable interrupts for A0[4] */
  if ( install_irq( ADC0_INT, (void *)ADC0Handler, HIGHEST_PRIORITY ) == FALSE )
  {
	return (FALSE);
  }
#endif
  return (TRUE);
}

/*****************************************************************************
** Function name:		ADC0Read
**
** Descriptions:		Read ADC0 channel
**
** parameters:			Channel number
** Returned value:		Value read, if interrupt driven, return channel #
** 
*****************************************************************************/
void ADC0Read( void )
{
#if !ADC_INTERRUPT_FLAG
  DWORD regVal, ADC_Data;
#endif

  AD0CR |= (1 << 24);
				/* switch channel,start A/D convert */
#if !ADC_INTERRUPT_FLAG
  while ( 1 )			/* wait until end of A/D convert */
  {
	regVal = *(volatile unsigned long *)(AD0_BASE_ADDR 
			+ ADC_OFFSET + ADC_INDEX * channelNum);
	/* read result of A/D conversion */
	if ( regVal & ADC_DONE )
	{
	  break;
	}
  }	
        
  AD0CR &= 0xF8FFFFFF;	/* stop ADC now */    
  if ( regVal & ADC_OVERRUN )	/* save data when it's not overrun, otherwise, return zero */
  {
	return ( 0 );
  }
  ADC_Data = ( regVal >> 6 ) & 0x3FF;
  return ( ADC_Data );	/* return A/D conversion value */
#else
  return ;
#endif
}

DWORD get_ADCval()
{
	ADC0Read();
	while (!ADC0IntDone);
	ADC0IntDone = 0;
	return ADC0Value;
}

#include "led.h"
#include "common.h"
void testADC(void)
{
	ADC0Read();
	while (!ADC0IntDone);
	ADC0IntDone = 0;
	printLED((BYTE)(0xFF >> (8 - (ADC0Value>>7))));
	busyWait(10);
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
