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
#include "uart2.h"
#include "common.h"
#include "led.h"
#include "cameraB.h"

volatile DWORD UART2Status;
volatile BYTE UART2TxEmpty = 1;
volatile BYTE UART2Buffer[UART2_BUFSIZE];
volatile DWORD UART2Count = 0;

/*****************************************************************************
 ** Function name:		UART2Handler
 **
 ** Descriptions:		UART2 interrupt handler
 **
 ** parameters:			None
 ** Returned value:		None
 **
 *****************************************************************************/
void UART2Handler (void)
{
	//printLED(0xAA);
	BYTE IIRValue, LSRValue; //value in IIR, LSR
	//BYTE cameraVaule;   moved to global

	IENABLE;				/* handles nested interrupt */
	IIRValue = U2IIR;

	IIRValue >>= 1;			/* skip pending bit in IIR */
	IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */

	//    RLS interrupt (U2IIR[3:1] = 011)
	//    --------------------------------
	//    The UART2 RLS interrupt (U2IIR[3:1] = 011) is the highest priority interrupt and is set
	//    whenever any one of four error conditions occur on the UART2 Rx input: overrun error
	//    (OE), parity error (PE), framing error (FE) and break interrupt (BI). The UART2 Rx error
	//    condition that set the interrupt can be observed via U2LSR[4:1]. The interrupt is cleared
	//    upon an U2LSR read.
	//    Action: --> check Line Status Register (U2LSR)

	if ( IIRValue == IIR_RLS )		/* Receive Line Status */
	{
		LSRValue = U2LSR;
		/* Receive Line Status */
		//handles interrupt by first checking if any of the RLS interrupts exist, then clears the RBR by
		//reading the RBR into cameraVaule and then disables interrupts then acknowledges it
		if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
		{
			/* There are errors or break interrupt */
			/* Read LSR will clear the interrupt */
			UART2Status = LSRValue;
			cameraValue = U2RBR;		/* cameraVaule read on RX to clear
					interrupt, then bail out */
			IDISABLE;
			VICVectAddr = 0;		/* Acknowledge Interrupt */
			return;
		}
		if ( LSRValue & LSR_RDR )	/* Receive Data Ready */
		{

			/* If no error on RLS, normal ready, save into the data buffer. */
			/* Note: read RBR will clear the interrupt */
			//16bit UART2 buffer.
			UART2Buffer[UART2Count] = U2RBR;
			UART2Count++;
			if ( UART2Count == UART2_BUFSIZE )
			{
				UART2Count = 0;		/* buffer overflow */
			}
		}
	}

	//    RDA interrupt (U2IIR[3:1] = 010)
	//    --------------------------------
	//    The UART2 RDA interrupt (U2IIR[3:1] = 010) shares the second level priority with the CTI
	//    interrupt (U2IIR[3:1] = 110). The RDA is activated when the UART2 Rx FIFO reaches the
	//    trigger level defined in U2FCR[7:6] and is reset when the UART2 Rx FIFO depth falls
	//    below the trigger level. When the RDA interrupt goes active, the CPU can read a block of
	//    data defined by the trigger level.


	else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
	{
		//printLED(255);
		/* Receive Data Available */
		UART2Buffer[UART2Count] = U2RBR;
		//cameraValue = UART2Buffer[UART2Count];
		if(UART2Buffer[cameraCount] == 0xD9){
			endFC = 1;
			busyWait(10);
			printLED(0xFF);
		}

		if(UART2Buffer[0] ==  76 && UART2Buffer[1] == 00 && UART2Buffer[2] == 34)
		{
			busyWait(10);
			camSize = 1;
			cameraValue = UART2Buffer[7];
			cameraValue+= UART2Buffer[8];
			printLED(0x11);
		}
		cameraCount++;
		UART2Count++;
		if ( UART2Count == UART2_BUFSIZE ) {
			printLED(7);
			UART2Count = 0;		/* buffer overflow */
		}
	}
	//    CTI interrupt (U2IIR[3:1] = 110)
	//    --------------------------------
	//    The CTI interrupt (U2IIR[3:1] = 110) is a second level interrupt and is set when the UART2
	//    Rx FIFO contains at least one character and no UART2 Rx FIFO activity has occurred in
	//    3.5 to 4.5 character times. Any UART2 Rx FIFO activity (read or write of UART2 RSR) will
	//    clear the interrupt. This interrupt is intended to flush the UART2 RBR after a message has
	//    been received that is not a multiple of the trigger level size. For example, if a peripheral
	//    wished to send a 105 character message and the trigger level was 10 characters, the CPU
	//    would receive 10 RDA interrupts resulting in the transfer of 100 characters and 1 to 5 CTI
	//    interrupts (depending on the service routine) resulting in the transfer of the remaining 5
	//    characters.

	else if ( IIRValue == IIR_CTI )	/* Character timeout indicator */
	{
		printLED(3);
		/* Character Time-out indicator */
		UART2Status |= 0x100;		/* Bit 9 as the CTI error */
	}

	/* THRE interrupt
	 * The UART2 THRE interrupt (U2IIR[3:1] = 001) is a third level interrupt and is activated
    		when the UART2 THR FIFO is empty provided certain initialization conditions have been
    		met. These initialization conditions are intended to give the UART2 THR FIFO a chance to
    		fill up with data to eliminate many THRE interrupts from occurring at system start-up. The
    		initialization conditions implement a one character delay minus the stop bit whenever
    		THRE=1 and there have not been at least two characters in the U2THR at one time since
    		the last THRE = 1 event. This delay is provided to give the CPU time to write data to
    		U2THR without a THRE interrupt to decode and service. A THRE interrupt is set
    		immediately if the UART2 THR FIFO has held two or more characters at one time and
    		currently, the U2THR is empty. The THRE interrupt is reset when a U2THR write occurs or
    		a read of the U2IIR occurs and the THRE is the highest interrupt (U2IIR[3:1] = 001).* */
	else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
	{
		printLED(4);

		LSRValue = U2LSR;		/* Check status in the LSR to see if
					valid data in U2THR or not */
		if ( LSRValue & LSR_THRE )
		{
			UART2TxEmpty = 1; //set uart THR is empty
		}
		else
		{
			printLED(5);
			UART2TxEmpty = 0; //uart THR is not empty
		}
	}

	IDISABLE;
	VICVectAddr = 0;		/* Acknowledge Interrupt */
}

/*****************************************************************************
 ** Function name:		UARTInit
 **
 ** Descriptions:		Initialize UART2 port, setup pin select,
 **				clock, parity, stop bits, FIFO, etc.
 **
 ** parameters:			UART baudrate
 ** Returned value:		true or false, return false only if the
 **				interrupt handler can't be installed to the
 **				VIC table
 **
 *****************************************************************************/
DWORD UART2Init( DWORD baudrate )
//DWORD UARTInit(  )

{
	DWORD Fdiv;
	PCONP |= 1<<24;
	PINSEL0 &= ~(0xF<<20);       /* Enable RxD3 and Not TxD1,  */
	PINSEL0 |= 0x5<<20;

	//LCR value determines format of data character that is to be transmitted or received
	/* 8 bits, no Parity, 1 Stop bit    */
	//DLAB = 1, enable access to DLL and DLM registers
	U2LCR = 0x83;
	Fdiv = ( Fpclk / 16 ) / baudrate ;	/*baud rate */
	U2DLM = Fdiv / 256;
	U2DLL = Fdiv % 256;
	//U2DLL = 0x4C;
	//U2DLM = 0x0;
	//U2FDR = 0x21;
	U2LCR = 0x03;       /*DLAB = 0 */
	U2FCR = 0x47;		/* Enable and reset TX and RX FIFO. */
	U2FCR = 0x40;
	if ( install_irq( UART2_INT, (void *)UART2Handler, HIGHEST_PRIORITY+1 ) == FALSE )
	{
		return (FALSE);
	}

	U2IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART2 interrupt */
	return (TRUE);
}


/*****************************************************************************
** Function name:		UARTSend
**
** Descriptions:		Send a block of data to the UART 0 port based
**						on the data length
**
** parameters:			portNum, buffer pointer, and data length
** Returned value:		None
**
*****************************************************************************/
void UART2Send( BYTE *BufferPtr, DWORD Length )
{
      while ( Length != 0 )
    {
	  /* THRE status, contain valid data */
	  while ( !(UART2TxEmpty & 0x01) );
	  U2THR = *BufferPtr;
	  UART2TxEmpty = 0;	/* not empty in the THR until it shifts out */
	  BufferPtr++;
	  Length--;
	}
  return;
}


/******************************************************************************
 **                            End Of File
 ******************************************************************************/
