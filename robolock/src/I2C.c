/*****************************************************************************
 *   i2c.c:  I2C C file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.19  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "LPC23xx.h"                        /* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "I2C.h"
#include "led.h"
#include "common.h"

volatile DWORD I2CMasterState = I2C_IDLE;
volatile DWORD I2CSlaveState = I2C_IDLE;

volatile DWORD I2CCmd;
volatile DWORD I2CMode;
extern volatile BYTE I2CMasterBuffer[BUFSIZE];
volatile BYTE I2CSlaveBuffer[BUFSIZE];
volatile DWORD I2CCount = 0;
volatile DWORD I2CReadLength;
volatile DWORD I2CWriteLength;

volatile DWORD RdIndex = 0;
volatile DWORD WrIndex = 0;

/* 
From device to device, the I2C communication protocol may vary, 
in the example below, the protocol uses repeated start to read data from or 
write to the device:
For master read: the sequence is: STA,Addr(W),offset,RE-STA,Addr(r),data...STO 
for master write: the sequence is: STA,Addr(W),length,RE-STA,Addr(w),data...STO
Thus, in state 8, the address is always WRITE. in state 10, the address could 
be READ or WRITE depending on the I2CCmd.
*/   

/*****************************************************************************
** Function name:		I2C1MasterHandler
**
** Descriptions:		I2C1 interrupt handler, deal with master mode
**				only.
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
//extern void I2C1MasterHandler( void ) __irq;
void I2C1MasterHandler(void)// __irq
{
  BYTE StatValue;
 // printLED(0xF0);
 // busyWait(100);

  /* this handler deals with master read and master write only */
  StatValue = I21STAT;
  IENABLE;				/* handles nested interrupt */
  switch ( StatValue )
  {
	case 0x08:			/* A Start condition is issued. */
	I21DAT = I2CMasterBuffer[0];
	I21CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	I2CMasterState = I2C_STARTED;
	break;
	
	case 0x10:			/* A repeated started is issued */
	if ( I2CCmd == LCD_TEMP )
	{
	  I21DAT = I2CMasterBuffer[2];
	}
	I21CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	I2CMasterState = I2C_RESTARTED;
	break;
	
	case 0x18:			/* Regardless, it's a ACK */
	if ( I2CMasterState == I2C_STARTED )
	{
	  I21DAT = I2CMasterBuffer[1+WrIndex];
	  WrIndex++;
	  I2CMasterState = DATA_ACK;
	}
	I21CONCLR = I2CONCLR_SIC;
	break;
	
	case 0x28:	/* Data byte has been transmitted, regardless ACK or NACK */
	case 0x30:
	if ( WrIndex != I2CWriteLength )
	{   
	  I21DAT = I2CMasterBuffer[1+WrIndex]; /* this should be the last one */
	  WrIndex++;
	  if ( WrIndex != I2CWriteLength )
	  {   
		I2CMasterState = DATA_ACK;
	  }
	  else
	  {
		I2CMasterState = DATA_NACK;
		if ( I2CReadLength != 0 )
		{
		  I21CONSET = I2CONSET_STA;	/* Set Repeated-start flag */
		  I2CMasterState = I2C_REPEATED_START;
		}
	  }
	}
	else
	{
	  if ( I2CReadLength != 0 )
	  {
		I21CONSET = I2CONSET_STA;	/* Set Repeated-start flag */
		I2CMasterState = I2C_REPEATED_START;
	  }
	  else
	  {
		I2CMasterState = DATA_NACK;
		I21CONSET = I2CONSET_STO;      /* Set Stop flag */
	  }
	}
	I21CONCLR = I2CONCLR_SIC;
	break;
	
	case 0x40:	/* Master Receive, SLA_R has been sent */
	I21CONSET = I2CONSET_AA;	/* assert ACK after data is received */
	I21CONCLR = I2CONCLR_SIC;
	break;
	
	case 0x50:	/* Data byte has been received, regardless following ACK or NACK */
	case 0x58:
	I2CMasterBuffer[3+RdIndex] = I21DAT;
	RdIndex++;
	if ( RdIndex != I2CReadLength )
	{   
	  I2CMasterState = DATA_ACK;
	}
	else
	{
	  RdIndex = 0;
	  I2CMasterState = DATA_NACK;
	}
	I21CONSET = I2CONSET_AA;	/* assert ACK after data is received */
	I21CONCLR = I2CONCLR_SIC;
	break;
	
	case 0x20:		/* regardless, it's a NACK */
	case 0x48:
	I21CONCLR = I2CONCLR_SIC;
	I2CMasterState = DATA_NACK;
	break;
	
	case 0x38:		/* Arbitration lost, in this example, we don't
					deal with multiple master situation */
	default:
	I21CONCLR = I2CONCLR_SIC;
	break;
  }
  IDISABLE;
  VICVectAddr = 0;		/* Acknowledge Interrupt */
}

/*****************************************************************************
** Function name:		I2CStart
**
** Descriptions:		Create I2C start condition, a timeout
**				value is set if the I2C never gets started,
**				and timed out. It's a fatal error. 
**
** parameters:			None
** Returned value:		true or false, return false if timed out
** 
*****************************************************************************/
DWORD I2CStart( void )
{
	printLED(0x04);
	busyWait(100);
  DWORD timeout = 0;
  DWORD retVal = FALSE;
 
  /*--- Issue a start condition ---*/
  I21CONSET = I2CONSET_STA;	/* Set Start flag */
    
  /*--- Wait until START transmitted ---*/
  while( 1 )
  {
	if ( I2CMasterState == I2C_STARTED )
	{
		printLED(0x05);
		busyWait(100);
	  retVal = TRUE;
	  break;	
	}
	if ( timeout >= MAX_TIMEOUT )
	{
		//test for timeout
		printLED(0xFF);
		busyWait(100);
	  retVal = FALSE;
	  break;
	}
	timeout++;
  }
  return( retVal );
}

/*****************************************************************************
** Function name:		I2CStop
**
** Descriptions:		Set the I2C stop condition, if the routine
**				never exit, it's a fatal bus error.
**
** parameters:			None
** Returned value:		true or never return
** 
*****************************************************************************/
DWORD I2CStop( void )
{
  I21CONSET = I2CONSET_STO;      /* Set Stop flag */
  I21CONCLR = I2CONCLR_SIC;  /* Clear SI flag */
            
  /*--- Wait for STOP detected ---*/
  while( I21CONSET & I2CONSET_STO );
  return TRUE;
}

/*****************************************************************************
** Function name:		I2CInit
**
** Descriptions:		Initialize I2C controller
**
** parameters:			I2c mode is either MASTER or SLAVE
** Returned value:		true or false, return false if the I2C
**				interrupt handler was not installed correctly
** 
*****************************************************************************/
DWORD I2CInit( DWORD I2cMode ) //0 slave 1 master
{
  PCONP |= (1 << 19);
 // PINSEL1 &= ~0x03C00000;
 // PINSEL1 |=  0x01400000;	/* set PIO0.27 and PIO0.28 to I2C1 SDA and SCK */
  PINSEL0 |= 0x0000000F;  /* function to 01 on both SDA and SCK. for I21 */

  /*--- Clear flags ---*/
  I21CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;

  /*--- Reset registers ---*/
  I21SCLL   = I2SCLL_SCLL;
  I21SCLH   = I2SCLH_SCLH;
  if ( I2cMode == I2CSLAVE )
  {
	I21ADR = LCD_ADDR;
  }    

  /* Install interrupt handler */	
  if ( install_irq( I2C1_INT, (void *)I2C1MasterHandler, HIGHEST_PRIORITY ) == FALSE )
  {
	  printLED(0x06);
	  busyWait(10000);
	return( FALSE );
  }
  I21CONSET = I2CONSET_I2EN;
  return( TRUE );
}

/*****************************************************************************
** Function name:		I2CEngine
**
** Descriptions:		The routine to complete a I2C transaction
**				from start to stop. All the intermitten
**				steps are handled in the interrupt handler.
**				Before this routine is called, the read
**				length, write length, I2C master buffer,
**				and I2C command fields need to be filled.
**				see i2cmst.c for more details. 
**
** parameters:			None
** Returned value:		true or false, return false only if the
**				start condition can never be generated and
**				timed out. 
** 
*****************************************************************************/
DWORD I2CEngine( void ) 
{
  I2CMasterState = I2C_IDLE;
  RdIndex = 0;
  WrIndex = 0;
  if ( I2CStart() != TRUE )
  {
	  printLED(0x0F);
	  busyWait(10000);
	I2CStop();
	return ( FALSE );
  }

  while ( 1 )
  {
	if ( I2CMasterState == DATA_NACK )
	{
	  I2CStop();
	  break;
	}
  }    
  return ( TRUE );      
}

/******************************************************************************
**                            End Of File
******************************************************************************/

