
/*****************************************************************************
 *   uart.h:  Header file for Philips LPC214x Family Microprocessors
 *
 *   Copyright(C) 2006, Philips Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2005.10.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __UART2_H
#define __UART2_H


#include "type.h"
#include "common.h"
#include "LPC23xx.h"                        /* LPC21xx definitions */
#include "target.h"
#include "irq.h"
#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

#define UART2_BUFSIZE		0x20


DWORD UART2Init( DWORD Baudrate );
//DWORD UARTInit(  );
void UART2Handler( void ) __irq;
void UART2Send(BYTE *, DWORD);

extern volatile DWORD UART2Status;
extern volatile BYTE UART2TxEmpty;
extern volatile BYTE UART2Buffer[UART2_BUFSIZE];
extern volatile DWORD UART2Count;


#endif /* end __UART_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
