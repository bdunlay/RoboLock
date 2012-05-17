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


#if 0
/******************************************************************************
 *
 * $RCSfile: $
 * $Revision: $
 *
 * This module provides interface routines to the LPC ARM UARTs.
 * Copyright 2004, R O SoftWare
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 * Modified by Martin Thomas for LPC23xx 24xx
 *****************************************************************************/

#include "LPC_REGS.h"
#include "sys_config.h"

#include "uart.h"

#ifndef USHRT_MAX
#define USHRT_MAX 16
#endif

#if defined(UART0_TX_INT_MODE) || defined(UART0_RX_INT_MODE) || \
    defined(UART1_TX_INT_MODE) || defined(UART1_RX_INT_MODE)
#include "armVIC.h"
#include "uartISR.h"
#endif

#if UART0_SUPPORT
#ifdef UART0_RX_INT_MODE
uint8_t  uart0_rx_buffer[UART0_RX_BUFFER_SIZE];
uint16_t uart0_rx_insert_idx, uart0_rx_extract_idx;
#endif

#ifdef UART0_TX_INT_MODE
uint8_t  uart0_tx_buffer[UART0_TX_BUFFER_SIZE];
uint16_t uart0_tx_insert_idx, uart0_tx_extract_idx;
int      uart0_tx_running;
#endif
#endif

#if UART1_SUPPORT
#ifdef UART1_RX_INT_MODE
uint8_t  uart1_rx_buffer[UART1_RX_BUFFER_SIZE];
uint16_t uart1_rx_insert_idx, uart1_rx_extract_idx;
#endif

#ifdef UART1_TX_INT_MODE
uint8_t  uart1_tx_buffer[UART1_TX_BUFFER_SIZE];
uint16_t uart1_tx_insert_idx, uart1_tx_extract_idx;
int      uart1_tx_running;
#endif
#endif

#if UART0_SUPPORT

/******************************************************************************
 *
 * Function Name: uart0Init()
 *
 * Description:
 *    This function initializes the UART for async mode
 *
 * Calling Sequence:
 *    baudrate divisor - use UART_BAUD macro
 *    mode - see typical modes (uart.h)
 *    fmode - see typical fmodes (uart.h)
 *
 * Returns:
 *    void
 *
 * NOTE: uart0Init(UART_BAUD(9600), UART_8N1, UART_FIFO_8);
 *
 *****************************************************************************/
void uart0Init(uint16_t baud, uint8_t mode, uint8_t fmode)
{
  // set port pins for UART0
  // mthomas PINSEL0 = (PINSEL0 & ~U0_PINMASK) | U0_PINSEL;
  // set port pins for UART0
  // PINSEL0 = (PINSEL0 & ~U0_PINMASK) | U0_PINSEL;
  U0_TX_PINSEL_REG = ( U0_TX_PINSEL_REG & ~U0_TX_PINMASK ) | U0_TX_PINSEL;
  U0_RX_PINSEL_REG = ( U0_RX_PINSEL_REG & ~U0_RX_PINMASK ) | U0_RX_PINSEL;

  U0IER = 0x00;                         // disable all interrupts
  U0IIR;                                // clear interrupt ID
  U0RBR;                                // clear receive register
  U0LSR;                                // clear line status register

  // set the baudrate
  U0LCR = ULCR_DLAB_ENABLE;             // select divisor latches
  U0DLL = (uint8_t)baud;                // set for baud low byte
  U0DLM = (uint8_t)(baud >> 8);         // set for baud high byte

  // set the number of characters and other
  // user specified operating parameters
  U0LCR = (mode & ~ULCR_DLAB_ENABLE);
  U0FCR = fmode;

#if defined(UART0_TX_INT_MODE) || defined(UART0_RX_INT_MODE)
  // initialize the interrupt vector
  VICIntSelect    &= ~VIC_CHAN_TO_MASK(VIC_CHAN_NUM_UART0);
  VICIntEnClr      = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_UART0);
  VICVectAddr6     = (uint32_t)uart0ISR;
  VICVectPriority6 = 0xF;
  VICIntEnable     = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_UART0);
#if 0
  VICIntSelect &= ~VIC_BIT(VIC_UART0);  // UART0 selected as IRQ
  VICIntEnable = VIC_BIT(VIC_UART0);    // UART0 interrupt enabled
  VICVectCntl0 = VIC_ENABLE | VIC_UART0;
  VICVectAddr0 = (uint32_t)uart0ISR;    // address of the ISR
#endif
#ifdef UART0_TX_INT_MODE
  // initialize the transmit data queue
  uart0_tx_extract_idx = uart0_tx_insert_idx = 0;
  uart0_tx_running = 0;
#endif

#ifdef UART0_RX_INT_MODE
  // initialize the receive data queue
  uart0_rx_extract_idx = uart0_rx_insert_idx = 0;

  // enable receiver interrupts
  U0IER = UIER_RBR;
#endif
#endif
}





/******************************************************************************
 *
 * Function Name: uart0Putch()
 *
 * Description:
 *    This function puts a character into the UART output queue for
 *    transmission.
 *
 * Calling Sequence:
 *    character to be transmitted
 *
 * Returns:
 *    ch on success, -1 on error (queue full)
 *
 *****************************************************************************/
int uart0Putch(int ch)
{
#ifdef UART0_TX_INT_MODE
  uint16_t temp;
  unsigned cpsr;

  temp = (uart0_tx_insert_idx + 1) % UART0_TX_BUFFER_SIZE;

  if (temp == uart0_tx_extract_idx)
    return -1;                          // no room

  cpsr = disableIRQ();                  // disable global interrupts
  U0IER &= ~UIER_THRE;                  // disable TX interrupts
  restoreIRQ(cpsr);                     // restore global interrupts

  // check if in process of sending data
  if (uart0_tx_running)
    {
    // add to queue
    uart0_tx_buffer[uart0_tx_insert_idx] = (uint8_t)ch;
    uart0_tx_insert_idx = temp;
    }
  else
    {
    // set running flag and write to output register
    uart0_tx_running = 1;
    U0THR = (uint8_t)ch;
    }

  cpsr = disableIRQ();                  // disable global interrupts
  U0IER |= UIER_THRE;                   // enable TX interrupts
  restoreIRQ(cpsr);                     // restore global interrupts
#else
  while (!(U0LSR & ULSR_THRE))          // wait for TX buffer to empty
    continue;                           // also either WDOG() or swap()

  U0THR = (uint8_t)ch;
#endif
  return (uint8_t)ch;
}

/******************************************************************************
 *
 * Function Name: uart0Space()
 *
 * Description:
 *    This function gets the available space in the transmit queue
 *
 * Calling Sequence:
 *    void
 *
 * Returns:
 *    available space in the transmit queue
 *
 *****************************************************************************/
uint16_t uart0Space(void)
{
#ifdef UART0_TX_INT_MODE
  int space;

  if ((space = (uart0_tx_extract_idx - uart0_tx_insert_idx)) <= 0)
    space += UART0_TX_BUFFER_SIZE;

  return (uint16_t)(space - 1);
#else
  return USHRT_MAX;
#endif
}

/******************************************************************************
 *
 * Function Name: uart0Puts()
 *
 * Description:
 *    This function writes a NULL terminated 'string' to the UART output
 *    queue, returning a pointer to the next character to be written.
 *
 * Calling Sequence:
 *    address of the string
 *
 * Returns:
 *    a pointer to the next character to be written
 *    (\0 if full string is written)
 *
 *****************************************************************************/
const char *uart0Puts(const char *string)
{
  register char ch;

  while ((ch = *string) && (uart0Putch(ch) >= 0))
    string++;

  return string;
}

/******************************************************************************
 *
 * Function Name: uart0Write()
 *
 * Description:
 *    This function writes 'count' characters from 'buffer' to the UART
 *    output queue.
 *
 * Calling Sequence:
 *
 *
 * Returns:
 *    0 on success, -1 if insufficient room, -2 on error
 *    NOTE: if insufficient room, no characters are written.
 *
 *****************************************************************************/
int uart0Write(const char *buffer, uint16_t count)
{
#ifdef UART0_TX_INT_MODE
  if (count > uart0Space())
    return -1;
#endif
  while (count && (uart0Putch(*buffer++) >= 0))
    count--;

  return (count ? -2 : 0);
}



/******************************************************************************
 *
 * Function Name: uart0TxEmpty()
 *
 * Description:
 *    This function returns the status of the UART transmit data
 *    registers.
 *
 * Calling Sequence:
 *    void
 *
 * Returns:
 *    FALSE - either the tx holding or shift register is not empty
 *   !FALSE - if both the tx holding & shift registers are empty
 *
 *****************************************************************************/
int uart0TxEmpty(void)
{
  return (U0LSR & (ULSR_THRE | ULSR_TEMT)) == (ULSR_THRE | ULSR_TEMT);
}

/******************************************************************************
 *
 * Function Name: uart0TxFlush()
 *
 * Description:
 *    This function removes all characters from the UART transmit queue
 *    (without transmitting them).
 *
 * Calling Sequence:
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
void uart0TxFlush(void)
{
#ifdef UART0_TX_INT_MODE
  unsigned cpsr;

  U0FCR |= UFCR_TX_FIFO_RESET;          // clear the TX fifo

  // "Empty" the transmit buffer.
  cpsr = disableIRQ();                  // disable global interrupts
  U0IER &= ~UIER_THRE;                 // disable TX interrupts
  restoreIRQ(cpsr);                     // restore global interrupts
  uart0_tx_insert_idx = uart0_tx_extract_idx = 0;
#else
  U0FCR |= UFCR_TX_FIFO_RESET;          // clear the TX fifo
#endif
}

/******************************************************************************
 *
 * Function Name: uart0Getch()
 *
 * Description:
 *    This function gets a character from the UART receive queue
 *
 * Calling Sequence:
 *    void
 *
 * Returns:
 *    character on success, -1 if no character is available
 *
 *****************************************************************************/
int uart0Getch(void)
{
#ifdef UART0_RX_INT_MODE
  uint8_t ch;

  if (uart0_rx_insert_idx == uart0_rx_extract_idx) // check if character is available
    return -1;

  ch = uart0_rx_buffer[uart0_rx_extract_idx++]; // get character, bump pointer
  uart0_rx_extract_idx %= UART0_RX_BUFFER_SIZE; // limit the pointer
  return ch;
#else
  if (U0LSR & ULSR_RDR)                 // check if character is available
    return U0RBR;                       // return character

  return -1;
#endif
}

#endif


#if UART1_SUPPORT

/******************************************************************************
 *
 * Function Name: uart1Init()
 *
 * Description:
 *    This function initializes the UART for async mode
 *
 * Calling Sequence:
 *    baudrate divisor - use UART_BAUD macro
 *    mode - see typical modes (uart.h)
 *    fmode - see typical fmodes (uart.h)
 *
 * Returns:
 *    void
 *
 * NOTE: uart1Init(UART_BAUD(9600), UART_8N1, UART_FIFO_8);
 *
 *****************************************************************************/
void uart1Init(uint16_t baud, uint8_t mode, uint8_t fmode)
{
  // set port pins for UART1
  // mthomas PINSEL0 = (PINSEL0 & ~U1_PINMASK) | U1_PINSEL;
  U1_TX_PINSEL_REG = ( U1_TX_PINSEL_REG & ~U1_TX_PINMASK ) | U1_TX_PINSEL;
  U1_RX_PINSEL_REG = ( U1_RX_PINSEL_REG & ~U1_RX_PINMASK ) | U1_RX_PINSEL;

  U1IER = 0x00;                         // disable all interrupts
  U1IIR;                                // clear interrupt ID
  U1RBR;                                // clear receive register
  U1LSR;                                // clear line status register

  // set the baudrate
  U1LCR = ULCR_DLAB_ENABLE;             // select divisor latches
  U1DLL = (uint8_t)baud;                // set for baud low byte
  U1DLM = (uint8_t)(baud >> 8);         // set for baud high byte

  // set the number of characters and other
  // user specified operating parameters
  U1LCR = (mode & ~ULCR_DLAB_ENABLE);
  U1FCR = fmode;

#if defined(UART1_TX_INT_MODE) || defined(UART1_RX_INT_MODE)
  // initialize the interrupt vector
  VICIntSelect    &= ~VIC_CHAN_TO_MASK(VIC_CHAN_NUM_UART1);
  VICIntEnClr      = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_UART1);
  VICVectAddr7     = (uint32_t)uart1ISR;
  VICVectPriority7 = 0x0F;
  VICIntEnable     = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_UART1);
#if 0
  VICIntSelect &= ~VIC_BIT(VIC_UART1);  // UART1 selected as IRQ
  VICIntEnable = VIC_BIT(VIC_UART1);    // UART1 interrupt enabled
  VICVectCntl1 = VIC_ENABLE | VIC_UART1;
  VICVectAddr1 = (uint32_t)uart1ISR;    // address of the ISR
#endif


#ifdef UART1_TX_INT_MODE
  uart1_tx_extract_idx = uart1_tx_insert_idx = 0;
  uart1_tx_running = 0;
#endif

#ifdef UART1_RX_INT_MODE
  // initialize data queues
  uart1_rx_extract_idx = uart1_rx_insert_idx = 0;

  // enable receiver interrupts
  U1IER |= UIER_RBR;
#endif
#endif
}

/******************************************************************************
 *
 * Function Name: uart1Putch()
 *
 * Description:
 *    This function puts a character into the UART output queue for
 *    transmission.
 *
 * Calling Sequence:
 *    ch - character to be transmitted
 *
 * Returns:
 *    ch on success, -1 on error (queue full)
 *
 *****************************************************************************/
int uart1Putch(int ch)
{
#ifdef UART1_TX_INT_MODE
  uint16_t temp;
  unsigned cpsr;

  temp = (uart1_tx_insert_idx + 1) % UART1_TX_BUFFER_SIZE;

  if (temp == uart1_tx_extract_idx)
    return -1;                          // no room

  cpsr = disableIRQ();                  // disable global interrupts
  U1IER &= ~UIER_THRE;                 // disable TX interrupts
  restoreIRQ(cpsr);                     // restore global interrupts

  // check if in process of sending data
  if (uart1_tx_running)
    {
    // add to queue
    uart1_tx_buffer[uart1_tx_insert_idx] = (uint8_t)ch;
    uart1_tx_insert_idx = temp;
    }
  else
    {
    // set running flag and write to output register
    uart1_tx_running = 1;
    U1THR = (uint8_t)ch;
    }

  cpsr = disableIRQ();                  // disable global interrupts
  U1IER |= UIER_THRE;                   // enable TX interrupts
  restoreIRQ(cpsr);                     // restore global interrupts
#else
  while (!(U1LSR & ULSR_THRE))          // wait for TX buffer to empty
    continue;                           // also either WDOG() or swap()

  U1THR = (uint8_t)ch;
#endif
  return (uint8_t)ch;
}

int uart1Putch_block(int ch)
{
	while ( !uart1Space() ) {
		; // wait for space available
	}
	return uart1Putch(ch);
}

/******************************************************************************
 *
 * Function Name: uart1Space()
 *
 * Description:
 *    This function gets the available space in the transmit queue
 *
 * Calling Sequence:
 *    void
 *
 * Returns:
 *    available space in the transmit queue
 *
 *****************************************************************************/
uint16_t uart1Space(void)
{
#ifdef UART1_TX_INT_MODE
  int space;

  if ((space = (uart1_tx_extract_idx - uart1_tx_insert_idx)) <= 0)
    space += UART1_TX_BUFFER_SIZE;

  return (uint16_t)(space - 1);
#else
  return USHRT_MAX;
#endif
}

/******************************************************************************
 *
 * Function Name: uart1Puts()
 *
 * Description:
 *    This function writes a NULL terminated 'string' to the UART output
 *    queue, returning a pointer to the next character to be written.
 *
 * Calling Sequence:
 *    address of the string
 *
 * Returns:
 *    a pointer to the next character to be written
 *    (\0 if full string is written)
 *
 *****************************************************************************/
const char *uart1Puts(const char *string)
{
  register char ch;

  while ((ch = *string) && (uart1Putch(ch) >= 0))
    string++;

  return string;
}

const char *uart1Puts_block(const char *string)
{
  register char ch;

  while ((ch = *string) && (uart1Putch_block(ch) >= 0))
    string++;

  return string;
}

/******************************************************************************
 *
 * Function Name: uart1Write()
 *
 * Description:
 *    This function writes 'count' characters from 'buffer' to the UART
 *    output queue.
 *
 * Calling Sequence:
 *
 *
 * Returns:
 *    0 on success, -1 if insufficient room, -2 on error
 *    NOTE: if insufficient room, no characters are written.
 *
 *****************************************************************************/
int uart1Write(const char *buffer, uint16_t count)
{
#ifdef UART1_TX_INT_MODE
  if (count > uart1Space())
    return -1;
#endif
  while (count && (uart1Putch(*buffer++) >= 0))
    count--;

  return (count ? -2 : 0);
}

/******************************************************************************
 *
 * Function Name: uart1TxEmpty()
 *
 * Description:
 *    This function returns the status of the UART transmit data
 *    registers.
 *
 * Calling Sequence:
 *    void
 *
 * Returns:
 *    FALSE - either the tx holding or shift register is not empty
 *   !FALSE - if both the tx holding & shift registers are empty
 *
 *****************************************************************************/
int uart1TxEmpty(void)
{
  return (U1LSR & (ULSR_THRE | ULSR_TEMT)) == (ULSR_THRE | ULSR_TEMT);
}

/******************************************************************************
 *
 * Function Name: uart1TxFlush()
 *
 * Description:
 *    This function removes all characters from the UART transmit queue
 *    (without transmitting them).
 *
 * Calling Sequence:
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
void uart1TxFlush(void)
{
#ifdef UART1_TX_INT_MODE
  unsigned cpsr;

  U1FCR |= UFCR_TX_FIFO_RESET;          // clear the TX fifo

  // "Empty" the transmit buffer.
  cpsr = disableIRQ();                  // disable global interrupts
  U1IER &= ~UIER_THRE;                 // disable TX interrupts
  restoreIRQ(cpsr);                     // restore global interrupts
  uart1_tx_insert_idx = uart1_tx_extract_idx = 0;
#else
  U1FCR |= UFCR_TX_FIFO_RESET;          // clear the TX fifo
#endif
}

/******************************************************************************
 *
 * Function Name: uart1Getch()
 *
 * Description:
 *    This function gets a character from the UART receive queue
 *
 * Calling Sequence:
 *    void
 *
 * Returns:
 *    character on success, -1 if no character is available
 *
 *****************************************************************************/
int uart1Getch(void)
{
#ifdef UART1_RX_INT_MODE
  uint8_t ch;

  if (uart1_rx_insert_idx == uart1_rx_extract_idx) // check if character is available
    return -1;

  ch = uart1_rx_buffer[uart1_rx_extract_idx++]; // get character, bump pointer
  uart1_rx_extract_idx %= UART1_RX_BUFFER_SIZE; // limit the pointer
  return ch;
#else
  if (U1LSR & ULSR_RDR)                 // check if character is available
    return U1RBR;                       // return character

  return -1;
#endif
}

#endif


