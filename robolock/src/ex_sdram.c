/*****************************************************************************
 *   ex_sdram.c:  External SDRAM memory module file for NXP LPC24xx Family 
 *   Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2007.01.13  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"				/* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "target.h"
#include "timer.h"
#include "ex_sdram.h"

#include "uart.h"
#include "led.h"
#include "common.h"

/*****************************************************************************
** Function name:		SDRAMInit
**
** Descriptions:		Initialize external SDRAM memory Samsung 
**						K4S561632H, 256Mbit(4M x 16 bit). The same
**						code can be used for the Micron's MT48LC16M 
** 						For more info. regarding the details of the 
**						SDRAMs, go to their website for data sheet.  
**
** parameters:			None			 
** 						
** Returned value:		None
** 
*****************************************************************************/
void SDRAMInit( void )
{
  DWORD i, dummy = dummy;

  /*************************************************************************
  * Initialize EMC and SDRAM
  *************************************************************************/
//  SCS     |= 0x00000002;		/* Reset EMC */ 
  EMC_CTRL = 0x00000001;		/*Disable Address mirror*/
  PCONP   |= 0x00000800;		/* Turn On EMC PCLK */
  //PINSEL4 |= 0x50000000;		/* set pins to CS[2:3] */
  PINSEL5 |= 0x05050555;		/* set pins to CASn, RASn, CLKOUT[1:0], DYCS[1:0], CKEOUT[1:0], DQMOUT[1:0] */
  PINSEL6 |= 0x55555555;		/* set pins to D[0:15] (data) */
  PINSEL8 |= 0x55555555;		/* set pins to A[0:15] (address) */
  PINSEL9 |= 0x00555555;  		/* set pins to A[16:23], OEn, WEn, BLS[1:0] */
  
  EMC_DYN_RP     = 1;		/* command period: 2 clock cycles (1 clock cycle @ 72Mhz = 13.89ns) */
  EMC_DYN_RAS    = 3;		/* RAS command period: 4 clock cycles */
  EMC_DYN_SREX   = 5;		/* Self-refresh period: 6 clock cycles */
  EMC_DYN_APR    = 2;		/* Data out to active: 3 clock cycles */
  EMC_DYN_DAL    = 4;		/* Data in to active: 5 clock cycles */
  EMC_DYN_WR     = 1;		/* Write recovery: 2 clock cycles */
  EMC_DYN_RC     = 4;		/* Active to Active cmd: 5 clock cycles */
  EMC_DYN_RFC    = 4;		/* Auto-refresh: 5 clock cycles */
  EMC_DYN_XSR    = 5;		/* Exit self-refresh: 6 clock cycles */
  EMC_DYN_RRD    = 1;		/* Active bank A->B: 2 clock cycles */
  EMC_DYN_MRD    = 1;		/* Load Mode to Active cmd: 2 clock cycles */

  EMC_DYN_RD_CFG = 1;		/* Command delayed strategy */

  /* Default setting, RAS latency 3 CCLKs, CAS latenty 3 CCLKs. */
  EMC_DYN_RASCAS0 = 0x00000303;

  /* 128MB, 8Mx16, 4 banks, row=12, column=9 */
  EMC_DYN_CFG0 = 0x00000480;
  delayMs(1, 100);			/* use timer 1, wait for 100ms */

  /* Mem clock enable, CLKOUT runs, send command: NOP */
  EMC_DYN_CTRL = 0x00000183;
  delayMs(1, 200);			/* use timer 1, wait for 200ms */
    
  /* Send command: PRECHARGE-ALL, shortest possible refresh period */
  EMC_DYN_CTRL = 0x00000103;

  /* set 32 CCLKs between SDRAM refresh cycles */
  EMC_DYN_RFSH = 0x00000002;
  for(i = 0; i < 0x40; i++);	/* wait 128 AHB clock cycles */
    
  /* set 71 x 16CCLKs=1136CCLK=15.8us between SDRAM refresh cycles */
  EMC_DYN_RFSH = 71;
    
  /* To set mode register in SDRAM, enter mode by issue
  MODE command, after finishing, bailout and back to NORMAL mode. */    
  /* Mem clock enable, CLKOUT runs, send command: MODE */
  EMC_DYN_CTRL = 0x00000083;
  
  /* Set mode register in SDRAM */
  /* Mode regitster table for Micron's MT48LCxx */
  /* bit 9:   Programmed burst length(0)
     bit 8~7: Normal mode(0)
	 bit 6~4: CAS latency 3
	 bit 3:   Sequential(0)
	 bit 2~0: Burst length is 8
	 row position is 12 */
  dummy = *((volatile DWORD *)(SDRAM_BASE_ADDR | (0x33 << 12)));
  
  EMC_DYN_CTRL = 0x00000000;	  /* Send command: NORMAL */

  EMC_DYN_CFG0 |= 0x00080000;	  /* Enable buffer */
  delayMs(1, 1);				  /* Use timer 1 */
  return;

}

void testSDRAM(void)
{
	  volatile DWORD *wr_ptr;
	  volatile BYTE *char_wr_ptr;
	  volatile WORD *short_wr_ptr;
	  DWORD  i;

	  printLED(0x01);

	  /* initialize memory */
	  //SDRAMInit();
	  printLED(0x02);

	  wr_ptr = (DWORD *)SDRAM_BASE_ADDR;
	  char_wr_ptr = (BYTE *)wr_ptr;
	  /* Clear content before 8 bit access test */
	  for ( i= 0; i < SDRAM_SIZE/4; i++ )
	  {
		*wr_ptr++ = 0;
	  }
	  printLED(0x04);

	  /* 8 bit write */
	  for (i=0; i<SDRAM_SIZE; i++)
	  {
		*char_wr_ptr++ = 0xAA;
		*char_wr_ptr++ = 0x55;
		*char_wr_ptr++ = 0x5A;
		*char_wr_ptr++ = 0xA5;
	  }
	  printLED(0x08);

	  /* verifying */
	  wr_ptr = (DWORD *)SDRAM_BASE_ADDR;
	  for ( i= 0; i < SDRAM_SIZE/4; i++ )
	  {
		//printLED(0xFF >> i/(SDRAM_SIZE/32));
		if ( *wr_ptr != 0xA55A55AA )	/* be aware of endianess */
		{
			printLED(0xAA);
			busyWait(50);
			printLED(0x55);
			busyWait(50);
			printLED(0xAA);
			busyWait(50);
			printLED(0x55);
			busyWait(300);
			printLED(*wr_ptr);
			busyWait(300);
			printLED(*wr_ptr >> 8);
			busyWait(300);
			printLED(*wr_ptr >> 16);
			busyWait(300);
			printLED(*wr_ptr >> 24);
			busyWait(300);
			break;
		}
		wr_ptr++;
	  }
	  printLED(0x0F);

	  wr_ptr = (DWORD *)SDRAM_BASE_ADDR;
	  short_wr_ptr = (WORD *)wr_ptr;
	  /* Clear content before 16 bit access test */
	  for ( i= 0; i < SDRAM_SIZE/4; i++ )
	  {
		*wr_ptr++ = 0;
	  }
	  printLED(0x1F);

	  /* 16 bit write */
	  for (i=0; i<(SDRAM_SIZE/2); i++)
	  {
		*short_wr_ptr++ = 0x5AA5;
		*short_wr_ptr++ = 0xAA55;
	  }
	  printLED(0x2F);

	  /* Verifying */
	  wr_ptr = (DWORD *)SDRAM_BASE_ADDR;
	  for ( i= 0; i < SDRAM_SIZE/4; i++ )
	  {
		//printLED(0xFF >> i/(SDRAM_SIZE/32));
		if ( *wr_ptr != 0xAA555AA5 )	/* be aware of endianess */
		{
			printLED(0xCC);
			busyWait(50);
			printLED(0x33);
			busyWait(50);
			printLED(0xCC);
			busyWait(50);
			printLED(0x33);
			busyWait(50);
			break;
		}
		wr_ptr++;
	  }
	  printLED(0xFF);
}

void testSDRAM_simple(void)
{
	volatile DWORD *wr_ptr;
	volatile BYTE *char_wr_ptr;
	volatile WORD *short_wr_ptr;
	DWORD  i;

	printLED(0x01);
	wr_ptr = (DWORD *)SDRAM_BASE_ADDR;
	*wr_ptr = 0x00000000;
	*wr_ptr = 0xA55A55AA;
	if ( *wr_ptr != 0xA55A55AA )
	{
		printLED(0xFF);
		busyWait(50);
		printLED(0x00);
		busyWait(50);
		printLED(0xFF);
		busyWait(50);
		printLED(0x00);
		busyWait(50);
	}

	printLED(0x02);

	wr_ptr = (DWORD *)SDRAM_BASE_ADDR;
	char_wr_ptr = (BYTE *)wr_ptr;
	/* Clear content before 8 bit access test */
	for ( i= 0; i < 32; i++ )
	{
		*wr_ptr++ = 0;
	}
	printLED(0x04);

	/* 8 bit write */
	for (i=0; i < 32; i++)
	{
		*char_wr_ptr++ = i;
		*char_wr_ptr++ = i;
		*char_wr_ptr++ = i;
		*char_wr_ptr++ = i;
	}
	printLED(0x08);

	/* verifying */
	wr_ptr = (DWORD *)SDRAM_BASE_ADDR;
	for ( i= 0; i < 32; i++ )
	{
		if ( *wr_ptr != (i | i<<8 | i<<16 | i<<24) )
		{
			printLED(i);
			busyWait(300);
			printLED(*wr_ptr);
			busyWait(300);
			printLED(*wr_ptr >> 8);
			busyWait(300);
			printLED(*wr_ptr >> 16);
			busyWait(300);
			printLED(*wr_ptr >> 24);
			busyWait(300);
			break;
		}
		wr_ptr++;
	}
	printLED(0x0F);
	busyWait(100);
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
