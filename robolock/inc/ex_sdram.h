/*****************************************************************************
 *   ex_sdram.h:  Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2007.01.10  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __EX_SDRAM_H 
#define __EX_SDRAM_H

/* If FLASH debugging is enabled(1), the result of the SDRAM test
will be dumped to the UART, a PC terminal will be needed, setting
as baudrate 115200/data 8 bits/No parity/1 stop bit/no handshake; 
if disabled(0), the code will be executed without terminal display. */
#define FLASH_DEBUG			0

/*****************************************************************************
 * Defines and typedefs
 ****************************************************************************/
#define SDRAM_BASE_ADDR		0xA0000000
#define SDRAM_SIZE			0x02000000

extern void SDRAMInit( void );

#endif /* end __EX_SDRAM_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
