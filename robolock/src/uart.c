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
#include "uart.h"
#include "common.h"

volatile DWORD UART0Status;
volatile BYTE UART0TxEmpty = 1;
/*volatile*/ BYTE UART0Buffer[UART_BUFSIZE];
volatile DWORD UART0Count = 0;

/*****************************************************************************
** Function name:		UART0Handler
**
** Descriptions:		UART0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void UART0Handler (void)
{
    BYTE IIRValue, LSRValue; //value in IIR, LSR
    BYTE Dummy;

    IENABLE;				/* handles nested interrupt */
    IIRValue = U0IIR;

    IIRValue >>= 1;			/* skip pending bit in IIR */
    IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */

//    RLS interrupt (U0IIR[3:1] = 011)
//    --------------------------------
//    The UART0 RLS interrupt (U0IIR[3:1] = 011) is the highest priority interrupt and is set
//    whenever any one of four error conditions occur on the UART0 Rx input: overrun error
//    (OE), parity error (PE), framing error (FE) and break interrupt (BI). The UART0 Rx error
//    condition that set the interrupt can be observed via U0LSR[4:1]. The interrupt is cleared
//    upon an U0LSR read.
//    Action: --> check Line Status Register (U0LSR)

    if ( IIRValue == IIR_RLS )		/* Receive Line Status */
    {
	LSRValue = U0LSR;
	/* Receive Line Status */
	//handles interrupt by first checking if any of the RLS interrupts exist, then clears the RBR by
	//reading the RBR into DUMMY and then disables interrupts then acknowledges it
	if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
	{
	    /* There are errors or break interrupt */
	    /* Read LSR will clear the interrupt */
	    UART0Status = LSRValue;
	    Dummy = U0RBR;		/* Dummy read on RX to clear
					interrupt, then bail out */
	    IDISABLE;
	    VICVectAddr = 0;		/* Acknowledge Interrupt */
	    return;
	}
	if ( LSRValue & LSR_RDR )	/* Receive Data Ready */
	{
	    /* If no error on RLS, normal ready, save into the data buffer. */
	    /* Note: read RBR will clear the interrupt */
		//16bit UART0 buffer.
	    UART0Buffer[UART0Count] = U0RBR;
	    UART0Count++;
	    if ( UART0Count == UART_BUFSIZE )
	    {
		UART0Count = 0;		/* buffer overflow */
	    }
	}
    }

//    RDA interrupt (U0IIR[3:1] = 010)
//    --------------------------------
//    The UART0 RDA interrupt (U0IIR[3:1] = 010) shares the second level priority with the CTI
//    interrupt (U0IIR[3:1] = 110). The RDA is activated when the UART0 Rx FIFO reaches the
//    trigger level defined in U0FCR[7:6] and is reset when the UART0 Rx FIFO depth falls
//    below the trigger level. When the RDA interrupt goes active, the CPU can read a block of
//    data defined by the trigger level.


    else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
    {
	/* Receive Data Available */
	UART0Buffer[UART0Count] = U0RBR;
	UART0Count++;
	if ( UART0Count == UART_BUFSIZE )
	{
	    UART0Count = 0;		/* buffer overflow */
	}
    }
//    CTI interrupt (U0IIR[3:1] = 110)
//    --------------------------------
//    The CTI interrupt (U0IIR[3:1] = 110) is a second level interrupt and is set when the UART0
//    Rx FIFO contains at least one character and no UART0 Rx FIFO activity has occurred in
//    3.5 to 4.5 character times. Any UART0 Rx FIFO activity (read or write of UART0 RSR) will
//    clear the interrupt. This interrupt is intended to flush the UART0 RBR after a message has
//    been received that is not a multiple of the trigger level size. For example, if a peripheral
//    wished to send a 105 character message and the trigger level was 10 characters, the CPU
//    would receive 10 RDA interrupts resulting in the transfer of 100 characters and 1 to 5 CTI
//    interrupts (depending on the service routine) resulting in the transfer of the remaining 5
//    characters.

    else if ( IIRValue == IIR_CTI )	/* Character timeout indicator */
    {
	/* Character Time-out indicator */
	UART0Status |= 0x100;		/* Bit 9 as the CTI error */
    }

    /* THRE interrupt
    	 * The UART0 THRE interrupt (U0IIR[3:1] = 001) is a third level interrupt and is activated
    		when the UART0 THR FIFO is empty provided certain initialization conditions have been
    		met. These initialization conditions are intended to give the UART0 THR FIFO a chance to
    		fill up with data to eliminate many THRE interrupts from occurring at system start-up. The
    		initialization conditions implement a one character delay minus the stop bit whenever
    		THRE=1 and there have not been at least two characters in the U0THR at one time since
    		the last THRE = 1 event. This delay is provided to give the CPU time to write data to
    		U0THR without a THRE interrupt to decode and service. A THRE interrupt is set
    		immediately if the UART0 THR FIFO has held two or more characters at one time and
    		currently, the U0THR is empty. The THRE interrupt is reset when a U0THR write occurs or
    		a read of the U0IIR occurs and the THRE is the highest interrupt (U0IIR[3:1] = 001).* */
    else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
    {

	LSRValue = U0LSR;		/* Check status in the LSR to see if
					valid data in U0THR or not */
	if ( LSRValue & LSR_THRE )
	{
	    UART0TxEmpty = 1; //set uart THR is empty
	}
	else
	{
	    UART0TxEmpty = 0; //uart THR is not empty
	}
    }

    IDISABLE;
    VICVectAddr = 0;		/* Acknowledge Interrupt */
}

/*****************************************************************************
** Function name:		UARTInit
**
** Descriptions:		Initialize UART0 port, setup pin select,
**				clock, parity, stop bits, FIFO, etc.
**
** parameters:			UART baudrate
** Returned value:		true or false, return false only if the
**				interrupt handler can't be installed to the
**				VIC table
**
*****************************************************************************/
DWORD UARTInit( DWORD baudrate )
//DWORD UARTInit(  )

{
    DWORD Fdiv;

    PINSEL0 &= ~0x000000F0;       /* Enable RxD1 and TxD1, RxD0 and TxD0   */
    PINSEL0 |= 0x00000050;
    //LCR value determines format of data character that is to be transmitted or received
    /* 8 bits, no Parity, 1 Stop bit    */
    //DLAB = 1, enable access to DLL and DLM registers
    U0LCR = 0x83;
    Fdiv = ( Fpclk / 16 ) / baudrate ;	/*baud rate */
//    U0DLM = Fdiv / 256;
//    U0DLL = Fdiv % 256;
    U0DLL = 0x4C;
    U0DLM = 0x0;
    U0FDR = 0x21;
    U0LCR = 0x03;       /*DLAB = 0 */
    //U0FCR = 0x47;		/* Enable and reset TX and RX FIFO. */
    U0FCR = 0x40;
    if ( install_irq( UART0_INT, (void *)UART0Handler, HIGHEST_PRIORITY+1 ) == FALSE )
    {
	return (FALSE);
    }

    U0IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART0 interrupt */
    return (TRUE);
}

/*****************************************************************************
** Function name:		UARTSend
**
** Descriptions:		Send a block of data to the UART 0 port based
**				on the data length
**
** parameters:			buffer pointer, and data length
** Returned value:		None
**
*****************************************************************************/
void UARTSend(BYTE *BufferPtr, DWORD Length )
//void UARTSend()
{
	while ( *BufferPtr != 0 )
	    {
			while ( !(UART0TxEmpty & 0x01) );	/* THRE status, contain valid
								data */
			U0THR = *BufferPtr;
			UART0TxEmpty = 0;	/* not empty in the THR until it shifts out */
			BufferPtr++;
			Length--;
	    }
	    return;

}

void UARTSendChar(BYTE ch)
{
	while ( !(UART0TxEmpty & 0x01) );	/* THRE status, contain valid
						data */
	U0THR = ch;
	UART0TxEmpty = 0;	/* not empty in the THR until it shifts out */
}

void UARTSendHexWord(WORD hex)
{
	BYTE i;
	BYTE temp;
	for (i=0; i<4; i++)
	{
		temp = hexToChar((hex & 0xF000) >> 12);
		while ( !(UART0TxEmpty & 0x01) );	/* THRE status, contain valid
										data */
		U0THR = temp;
		UART0TxEmpty = 0;	/* not empty in the THR until it shifts out */
		hex = hex << 4;
	}
}

void testUART(void)
{
	UARTSend((BYTE*)"Hello world\n\r", 13);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
