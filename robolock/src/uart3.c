/*****************************************************************************
 *   uart.c:  UART API file for Philips LPC214x Family Microprocessors
 *
 *   Copyright(C) 2006, Philips Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2005.10.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"                        /* LPC21xx definitions */
#include "type.h"
#include "target.h"
#include "irq.h"
#include "uart3.h"
#include "common.h"
#include "led.h"

volatile DWORD UART3Status;
volatile BYTE UART3TxEmpty = 1;
volatile BYTE UART3Buffer[UART_BUFSIZE];
volatile DWORD UART3Count = 0;

/*****************************************************************************
** Function name:		UART3Handler
**
** Descriptions:		UART3 interrupt handler
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void UART3Handler (void)
{
	//printLED(0xAA);
    BYTE IIRValue, LSRValue; //value in IIR, LSR
 //   BYTE dummy3;   moved to global

    IENABLE;				/* handles nested interrupt */
    IIRValue = U3IIR;

    IIRValue >>= 1;			/* skip pending bit in IIR */
    IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */

//    RLS interrupt (U3IIR[3:1] = 011)
//    --------------------------------
//    The UART3 RLS interrupt (U3IIR[3:1] = 011) is the highest priority interrupt and is set
//    whenever any one of four error conditions occur on the UART3 Rx input: overrun error
//    (OE), parity error (PE), framing error (FE) and break interrupt (BI). The UART3 Rx error
//    condition that set the interrupt can be observed via U3LSR[4:1]. The interrupt is cleared
//    upon an U3LSR read.
//    Action: --> check Line Status Register (U3LSR)

    if ( IIRValue == IIR_RLS )		/* Receive Line Status */
    {
		printLED(1);
	LSRValue = U3LSR;
	/* Receive Line Status */
	//handles interrupt by first checking if any of the RLS interrupts exist, then clears the RBR by
	//reading the RBR into dummy3 and then disables interrupts then acknowledges it
	if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
	{
	    /* There are errors or break interrupt */
	    /* Read LSR will clear the interrupt */
	    UART3Status = LSRValue;
	    dummy3 = U3RBR;		/* dummy3 read on RX to clear
					interrupt, then bail out */
	    IDISABLE;
	    VICVectAddr = 0;		/* Acknowledge Interrupt */
	    return;
	}
	if ( LSRValue & LSR_RDR )	/* Receive Data Ready */
	{

	    /* If no error on RLS, normal ready, save into the data buffer. */
	    /* Note: read RBR will clear the interrupt */
		//16bit UART3 buffer.
	    UART3Buffer[UART3Count] = U3RBR;
	    UART3Count++;
	    if ( UART3Count == UART_BUFSIZE )
	    {
		UART3Count = 0;		/* buffer overflow */
	    }
	}
    }

//    RDA interrupt (U3IIR[3:1] = 010)
//    --------------------------------
//    The UART3 RDA interrupt (U3IIR[3:1] = 010) shares the second level priority with the CTI
//    interrupt (U3IIR[3:1] = 110). The RDA is activated when the UART3 Rx FIFO reaches the
//    trigger level defined in U3FCR[7:6] and is reset when the UART3 Rx FIFO depth falls
//    below the trigger level. When the RDA interrupt goes active, the CPU can read a block of
//    data defined by the trigger level.


    else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
    {
		printLED(2);
	/* Receive Data Available */
	UART3Buffer[UART3Count] = U3RBR;
	dummy3 = UART3Buffer[UART3Count];
	UART3Count++;
	if ( UART3Count == UART_BUFSIZE )
	{
		printLED(7);
	    UART3Count = 0;		/* buffer overflow */
	}
    }
//    CTI interrupt (U3IIR[3:1] = 110)
//    --------------------------------
//    The CTI interrupt (U3IIR[3:1] = 110) is a second level interrupt and is set when the UART3
//    Rx FIFO contains at least one character and no UART3 Rx FIFO activity has occurred in
//    3.5 to 4.5 character times. Any UART3 Rx FIFO activity (read or write of UART3 RSR) will
//    clear the interrupt. This interrupt is intended to flush the UART3 RBR after a message has
//    been received that is not a multiple of the trigger level size. For example, if a peripheral
//    wished to send a 105 character message and the trigger level was 10 characters, the CPU
//    would receive 10 RDA interrupts resulting in the transfer of 100 characters and 1 to 5 CTI
//    interrupts (depending on the service routine) resulting in the transfer of the remaining 5
//    characters.

    else if ( IIRValue == IIR_CTI )	/* Character timeout indicator */
    {
		printLED(3);
	/* Character Time-out indicator */
	UART3Status |= 0x100;		/* Bit 9 as the CTI error */
    }

    /* THRE interrupt
    	 * The UART3 THRE interrupt (U3IIR[3:1] = 001) is a third level interrupt and is activated
    		when the UART3 THR FIFO is empty provided certain initialization conditions have been
    		met. These initialization conditions are intended to give the UART3 THR FIFO a chance to
    		fill up with data to eliminate many THRE interrupts from occurring at system start-up. The
    		initialization conditions implement a one character delay minus the stop bit whenever
    		THRE=1 and there have not been at least two characters in the U3THR at one time since
    		the last THRE = 1 event. This delay is provided to give the CPU time to write data to
    		U3THR without a THRE interrupt to decode and service. A THRE interrupt is set
    		immediately if the UART3 THR FIFO has held two or more characters at one time and
    		currently, the U3THR is empty. The THRE interrupt is reset when a U3THR write occurs or
    		a read of the U3IIR occurs and the THRE is the highest interrupt (U3IIR[3:1] = 001).* */
    else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
    {
		printLED(4);

	LSRValue = U3LSR;		/* Check status in the LSR to see if
					valid data in U3THR or not */
	if ( LSRValue & LSR_THRE )
	{
	    UART3TxEmpty = 1; //set uart THR is empty
	}
	else
	{
		printLED(5);
	    UART3TxEmpty = 0; //uart THR is not empty
	}
    }

    IDISABLE;
    VICVectAddr = 0;		/* Acknowledge Interrupt */
}

/*****************************************************************************
** Function name:		UARTInit
**
** Descriptions:		Initialize UART3 port, setup pin select,
**				clock, parity, stop bits, FIFO, etc.
**
** parameters:			UART baudrate
** Returned value:		true or false, return false only if the
**				interrupt handler can't be installed to the
**				VIC table
**
*****************************************************************************/
DWORD UART3Init( DWORD baudrate )
//DWORD UARTInit(  )

{
    DWORD Fdiv;
PCONP |= 1<<25;
    PINSEL1 &= ~0xF<<18;       /* Enable RxD3 and Not TxD1,  */
    PINSEL1 |= 0xC<<18;

    //LCR value determines format of data character that is to be transmitted or received
    /* 8 bits, no Parity, 1 Stop bit    */
    //DLAB = 1, enable access to DLL and DLM registers
    U3LCR = 0x83;
    Fdiv = ( Fpclk / 16 ) / baudrate ;	/*baud rate */
//    U3DLM = Fdiv / 256;
//    U3DLL = Fdiv % 256;
    U3DLL = 0x4C;
    U3DLM = 0x0;
    U3FDR = 0x21;
    U3LCR = 0x03;       /*DLAB = 0 */
    U3FCR = 0x47;		/* Enable and reset TX and RX FIFO. */
    U3FCR = 0x40;
    if ( install_irq( UART3_INT, (void *)UART3Handler, HIGHEST_PRIORITY+1 ) == FALSE )
    {
	return (FALSE);
    }

    U3IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART3 interrupt */
    return (TRUE);
}





/******************************************************************************
**                            End Of File
******************************************************************************/
