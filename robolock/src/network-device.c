/*****************************************************************************
 *   emac.c:  Ethernet module file for NXP LPC230x Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.09.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"                        /* LPC21xx definitions */
#include "type.h"
#include "target.h"
#include "irq.h"
#include "network-device.h"

volatile DWORD Duplex;
volatile DWORD Speed;

volatile DWORD RXOverrunCount = 0;
volatile DWORD RXErrorCount = 0;

volatile DWORD TXUnderrunCount = 0;
volatile DWORD TXErrorCount = 0;
volatile DWORD RxFinishedCount = 0;
volatile DWORD TxFinishedCount = 0;
volatile DWORD TxDoneCount = 0;
volatile DWORD RxDoneCount = 0;

volatile DWORD CurrentRxPtr = EMAC_RX_BUFFER_ADDR;
volatile DWORD ReceiveLength = 0;
volatile DWORD PacketReceived = FALSE;

#if ENABLE_WOL
volatile DWORD WOLCount = 0;
volatile DWORD WOLArrived = FALSE;
#endif


/******************************************************************************
** Function name:		EMAC_TxEnable/EMAC_TxDisable
**
** Descriptions:		EMAC TX API modules
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void EMAC_TxEnable( void )
{
  MAC_COMMAND |= 0x02;
  return;
}

void EMAC_TxDisable( void )
{
  MAC_COMMAND &= ~0x02;
  return;
}

/******************************************************************************
** Function name:		EMAC_RxEnable/EMAC_RxDisable
**
** Descriptions:		EMAC RX API modules
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void EMAC_RxEnable( void )
{
  MAC_COMMAND |= 0x01;
  MAC_MAC1 |= 0x01;
  return;    
}

void EMAC_RxDisable( void )
{
  MAC_COMMAND &= ~0x01;
  MAC_MAC1 &= ~0x01;
  return;
}

/******************************************************************************
** Function name:		EMACHandler
**
** Descriptions:		EMAC interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void EMACHandler (void)
 {
  volatile DWORD IntStatus;
  DWORD *rxptr;

  IENABLE;				/* handles nested interrupt */

  IntStatus = MAC_INTSTATUS;    
  if ( IntStatus != 0 )	/* At least one interrupt */
  {
#if ENABLE_WOL
	if ( IntStatus & EMAC_INT_WOL )
	{
	  MAC_INTCLEAR = EMAC_INT_WOL;
	  WOLCount++;
	  WOLArrived = TRUE;
	  /* the packet will be lost, no need to anything else, bail out */
	  IDISABLE;
	  VICVectAddr = 0;		/* Acknowledge Interrupt */
	  return;
	}
#endif
	if ( IntStatus & EMAC_INT_RXOVERRUN )
	{
	  MAC_INTCLEAR = EMAC_INT_RXOVERRUN;
	  RXOverrunCount++;
	  IDISABLE;
	  VICVectAddr = 0;		/* Acknowledge Interrupt */
	  return;
	}

	if ( IntStatus & EMAC_INT_RXERROR )
	{
	  MAC_INTCLEAR = EMAC_INT_RXERROR;
	  RXErrorCount++;
	  IDISABLE;
	  VICVectAddr = 0;		/* Acknowledge Interrupt */
	  return;
	}
	
	if ( IntStatus & EMAC_INT_RXFINISHED )
	{
	  MAC_INTCLEAR = EMAC_INT_RXFINISHED;
	  RxFinishedCount++;
	  /* Below should never happen or RX is seriously wrong */
	  while ( MAC_RXPRODUCEINDEX != (MAC_RXCONSUMEINDEX - 1) );
	}

	if ( IntStatus & EMAC_INT_RXDONE )
	{
	  MAC_INTCLEAR = EMAC_INT_RXDONE;
	  ReceiveLength = EMACReceive( rxptr );
	  PacketReceived = TRUE;
	  RxDoneCount++;
	}

	if ( IntStatus & EMAC_INT_TXUNDERRUN )
	{
	  MAC_INTCLEAR = EMAC_INT_TXUNDERRUN;
	  TXUnderrunCount++;
	  IDISABLE;
	  VICVectAddr = 0;		/* Acknowledge Interrupt */
	  return;
	}
	
	if ( IntStatus & EMAC_INT_TXERROR )
	{
	  MAC_INTCLEAR = EMAC_INT_TXERROR;
	  TXErrorCount++;
	  IDISABLE;
	  VICVectAddr = 0;		/* Acknowledge Interrupt */
	  return;
	}

	if ( IntStatus & EMAC_INT_TXFINISHED )
	{
	  MAC_INTCLEAR = EMAC_INT_TXFINISHED;
	  TxFinishedCount++;
	}

	if ( IntStatus & EMAC_INT_TXDONE )
	{
	  MAC_INTCLEAR = EMAC_INT_TXDONE;
	  TxDoneCount++;
	}
  }   
  IDISABLE;
  VICVectAddr = 0;		/* Acknowledge Interrupt */
  return;
}

/*****************************************************************************
** Function name:		WritePHY
**
** Descriptions:		Write Data to the PHY port
**
** parameters:			PHY register, write data
** Returned value:		None
** 
*****************************************************************************/
void WritePHY( DWORD PHYReg, DWORD PHYData )
{
  MAC_MCMD = 0x0000;			/* write command */
  MAC_MADR = PHY_ADDR | PHYReg;	/* [12:8] == PHY addr, [4:0]=register addr */
  MAC_MWTD = PHYData;
  while ( MAC_MIND != 0 );
  return;
}

/*****************************************************************************
** Function name:		ReadPHY
**
** Descriptions:		Read data from the PHY port
**
** parameters:			PHY register
** Returned value:		PHY data
** 
*****************************************************************************/
DWORD ReadPHY( DWORD PHYReg )
{
  MAC_MCMD = 0x0001;			/* read command */
  MAC_MADR = PHY_ADDR | PHYReg;	/* [12:8] == PHY addr, [4:0]= register addr */
  while ( (MAC_MIND & 0x05) != 0 );
  MAC_MCMD = 0x0000;
  return( MAC_MRDD );
}

/*****************************************************************************
** Function name:		PHYInit
**
** Descriptions:		initialize PHY port
**
** parameters:			PHYType
** Returned value:		None
** 
*****************************************************************************/
DWORD PHYInit( DWORD PHYType )
{
  DWORD i, regValue;
  DWORD timeout;

  /* MII configuration, I may just need to do either RMII or MII configuration, not both. */
  MAC_MCFG = 0x8018;	/* host clock divided by 20, no suppress preamble, no scan increment */
  for ( i = 0; i < 0x40; i++ );
  MAC_MCFG = 0x0018;	/* Apply a reset */
  MAC_MCMD = 0;	
#if RMII 
  /* RMII configuration */
  MAC_COMMAND |= 0x0200;
  /* PHY support: [8]=0 ->10 Mbps mode, =1 -> 100 Mbps mode */ 
  MAC_SUPP = 0x0100;	/* RMII setting, at power-on, default set to 100. */
#else
  MAC_COMMAND &= ~0x0200;
  MAC_SUPP = 0x0000;
#endif

  for ( i = 0; i < 0x100; i++ );	/* short delay */

  WritePHY( PHY_BMCR, BMCR_RESET );
  for ( i = 0; i < 0x20; i++ );	/* short delay */

  timeout = MII_BMSR_TIMEOUT * 4;
  while ( timeout != 0 )
  {
	regValue = ReadPHY( PHY_BMCR );
	if ( (regValue & BMCR_RESET) == 0x0000 )	
	{
	  break;		/* Reset self cleaned once the reset process is complete */	
	}
	timeout--;	
  }
  if ( timeout == 0 )
  {
	return ( FALSE );
  }

  if ( PHYType == NATIONAL_PHY )
  {
	/* check PHY IDs to make sure the reset takes place and PHY
	is in its default state. See National PHY DP83848 Users Manual 
	for more details */
	regValue = ReadPHY( PHY_PHYIDR1 );
	if ( (regValue & 0x2000) != 0x2000 )
	{
	  return ( FALSE );
	}

	regValue = ReadPHY( PHY_PHYIDR2 );
	if ( (regValue & 0x5C90) != 0x5C90 )
	{
	  return ( FALSE );
	}
  }
  else if ( PHYType == MICREL_PHY )
  {
	regValue = ReadPHY( PHY_PHYIDR1 );
	if ( (regValue & 0x00FF) != 0x0022 )
	{
	  return ( FALSE );
	}

	regValue = ReadPHY( PHY_PHYIDR2 );
	if ( (regValue & 0x1610) != 0x1610 )
	{
	  return ( FALSE );
	}
  }

  WritePHY( PHY_BMCR, BMCR_AN | BMCR_RE_AN ); /* auto negotiation, restart AN */
  /* RE_AN should be self cleared */
//	while ( ((regValue = ReadPHY( PHY_BMCR )) & BMCR_RE_AN) );

  if ( PHYType == NATIONAL_PHY )
  { 
	timeout = MII_BMSR_TIMEOUT;
	while ( timeout != 0 )
	{
	  regValue = ReadPHY( NSM_PHY_PHYSTS );
	  if ( (regValue & 0x0011) == 0x0011 )	
	  {
		break;		/* link established if bit 0 is set */
	  }
	  timeout--;
	}

	if ( timeout == 0 )
	{
	  return ( FALSE );
	}

	/* Link established from here on */
	if ( regValue & 0x04 )
	  Duplex = FULL_DUPLEX;
	else
	  Duplex = HALF_DUPLEX;

	if ( regValue & 0x02 )
	  Speed = SPEED_10;
	else
	  Speed = SPEED_100;
  }

  else if ( PHYType == MICREL_PHY )
  { 
	timeout = MII_BMSR_TIMEOUT;
	while ( timeout != 0 )
	{
	  regValue = ReadPHY( PHY_BMSR );
	  if ( (regValue & (BMSR_AUTO_DONE|BMSR_LINK_ESTABLISHED)) 
			== (BMSR_AUTO_DONE|BMSR_LINK_ESTABLISHED) )	
	  {
		break;		/* auto nego complete and link established if bit 2 
					and 5 are set */
	  }
	  timeout--;
	}
    if ( timeout == 0 )
	{
	  return ( FALSE );
	}

	regValue = ReadPHY( MIC_PHY_100BASE_PHY_CTRL );
	/* successful negotiations; update link info */
	regValue &= 0x001C;
	switch ( regValue )
	{
	  case 0x0004:
		Speed = SPEED_10;
		Duplex = HALF_DUPLEX;
	  break;
	  case 0x0008:
		Speed = SPEED_100;
		Duplex = HALF_DUPLEX;
	  break;
	  case 0x0014:
		Speed = SPEED_10;
		Duplex = FULL_DUPLEX;
	  break;
	  case 0x0018:
		Speed = SPEED_100;
		Duplex = FULL_DUPLEX;
	  break;
	  default:	// Should not come here, force to set default, 100 FULL_DUPLEX
		Speed = SPEED_100;
		Duplex = FULL_DUPLEX;
		break;
	}
  }
  return ( TRUE );
}

/*****************************************************************************
** Function name:		EMACTxDesciptorInit
**
** Descriptions:		initialize EMAC TX descriptor table
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void EMACTxDescriptorInit( void )
{
  DWORD i;
  DWORD *tx_desc_addr, *tx_status_addr;
   
  /*-----------------------------------------------------------------------------      
   * setup the Tx status,descriptor registers -- 
   * Note, the actual tx packet data is loaded into the ahb2_sram16k memory as part
   * of the simulation
   *----------------------------------------------------------------------------*/ 
  MAC_TXDESCRIPTOR = TX_DESCRIPTOR_ADDR;	/* Base addr of tx descriptor array */
  MAC_TXSTATUS = TX_STATUS_ADDR;		/* Base addr of tx status */
  MAC_TXDESCRIPTORNUM = EMAC_TX_DESCRIPTOR_COUNT - 1;	/* number of tx descriptors, 16 */

  for ( i = 0; i < EMAC_TX_DESCRIPTOR_COUNT; i++ )
  {
	tx_desc_addr = (DWORD *)(TX_DESCRIPTOR_ADDR + i * 8);	/* two words at a time, packet and control */
	*tx_desc_addr = (DWORD)(EMAC_TX_BUFFER_ADDR + i * EMAC_BLOCK_SIZE);
	*(tx_desc_addr+1) = (DWORD)(EMAC_TX_DESC_INT | (EMAC_BLOCK_SIZE - 1));	/* set size only */
  }
    
  for ( i = 0; i < EMAC_TX_DESCRIPTOR_COUNT; i++ )
  {
	tx_status_addr = (DWORD *)(TX_STATUS_ADDR + i * 4);	/* TX status, one word only, status info. */
	*tx_status_addr = (DWORD)0;		/* initially, set status info to 0 */
  }
  MAC_TXPRODUCEINDEX = 0x0;	/* TX descriptors point to zero */
  return;
}

/*****************************************************************************
** Function name:		EMACRxDesciptorInit
**
** Descriptions:		initialize EMAC RX descriptor table
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void EMACRxDescriptorInit( void )
{
  DWORD i;
  DWORD *rx_desc_addr, *rx_status_addr;
   
  /*-----------------------------------------------------------------------------      
   * setup the Rx status,descriptor registers -- 
   * Note, the actual rx packet data is loaded into the ahb2_sram16k memory as part
   * of the simulation
   *----------------------------------------------------------------------------*/ 
  MAC_RXDESCRIPTOR = RX_DESCRIPTOR_ADDR;	/* Base addr of rx descriptor array */
  MAC_RXSTATUS = RX_STATUS_ADDR;			/* Base addr of rx status */
  MAC_RXDESCRIPTORNUM = EMAC_RX_DESCRIPTOR_COUNT - 1;	/* number of rx descriptors, 16 */

  for ( i = 0; i < EMAC_RX_DESCRIPTOR_COUNT; i++ )
  {
	/* two words at a time, packet and control */
	rx_desc_addr = (DWORD *)(RX_DESCRIPTOR_ADDR + i * 8);
	*rx_desc_addr = (DWORD)(EMAC_RX_BUFFER_ADDR + i * EMAC_BLOCK_SIZE);
	*(rx_desc_addr+1) = (DWORD)(EMAC_RX_DESC_INT | (EMAC_BLOCK_SIZE - 1));	/* set size only */    
  }
 
  for ( i = 0; i < EMAC_RX_DESCRIPTOR_COUNT; i++ )
  {
	/* RX status, two words, status info. and status hash CRC. */
	rx_status_addr = (DWORD *)(RX_STATUS_ADDR + i * 8);	
	*rx_status_addr = (DWORD)0;	/* initially, set both status info and hash CRC to 0 */
	*(rx_status_addr+1) = (DWORD)0; 
  }
  MAC_RXCONSUMEINDEX = 0x0;	/* RX descriptor points to zero */
  return;
}

/*****************************************************************************
** Function name:		EMACInit
**
** Descriptions:		initialize EMAC port
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
DWORD EMACInit( void )
{
  DWORD regVal;
  DWORD i;

  /* turn on the ethernet MAC clock in PCONP, bit 30 */
  regVal = PCONP;
  regVal |= PCONP_EMAC_CLOCK;
  PCONP = regVal;

  for ( i = 0; i < 0x100; i++ );	/* short delay */
  /*------------------------------------------------------------------------------      
   * write to PINSEL2/3 to select the PHY functions on P1[17:0]
   *-----------------------------------------------------------------------------*/
  /* documentation needs to be updated */
#if RMII
  /* P1.6, ENET-TX_CLK, has to be set for EMAC to address a BUG in the engineering 
   version, even if this pin is not used for RMII interface. This bug has been fixed,
   and this port pin can be used as GPIO pin in the future release. */ 
  /* Unfortunately, this MCB2300 board still has the old eng. version LPC23xx chip
   on it. On the new rev.(xxAY, released on 06/22/2007), P1.6 should NOT be set.
  See errata for more details. */
  regVal = MAC_MODULEID;
  if ( regVal == PHILIPS_EMAC_MODULE_ID ) 
  {
	/* This is the rev."-" ID for the existing MCB2300 board,
	on rev. A, regVal should NOT equal to PHILIPS_EMAC_MODULE_ID,
	P1.6 should NOT be set. */
	PINSEL2 = 0x50151105;	/* selects P1[0,1,4,6,8,9,10,14,15] */
  }
  else
  {
	PINSEL2 = 0x50150105;	/* selects P1[0,1,4,8,9,10,14,15] */
  }
  PINSEL3 = 0x00000005;	/* selects P1[17:16] */
#else					/* else RMII, then it's MII mode */      
  PINSEL2 = 0x55555555;	/* selects P1[15:0] */
  PINSEL3 = 0x00000005;	/* selects P1[17:16] */
#endif

  /*-----------------------------------------------------------------------------      
   * write the MAC config registers
   *----------------------------------------------------------------------------*/
  MAC_MAC1 = 0xCF00;	/* [15],[14],[11:8] -> soft resets all MAC internal modules */
  MAC_COMMAND = 0x0038;	/* reset all datapaths and host registers */

  for ( i = 0; i < 0x04; i++ );	/* short delay after reset */
  MAC_MAC1 = 0x0;		/* deassert all of the above soft resets in MAC1 */

  EMAC_TxDisable();
  EMAC_RxDisable();
    
  MAC_MAC2 = 0x00;		/* initialize MAC2 register to default value */

  /* Non back to back inter-packet gap */
  MAC_IPGR = 0x0012;	/* use the default value recommended in the users manual */

  MAC_CLRT = 0x370F;	/* Use the default value in the users manual */
  MAC_MAXF = 0x0600;	/* Use the default value in the users manual */
    
  /* On Keil and Internal Engineering board, National PHY is used.
     On IAR and Embedded Artists board, Micrel PHY is used. However,
	 on IAR board and Embedded Artists board, due to the different
	 I/O pin setting, the PHY addresses are different. On IAR board,
	 Internal Engineering board, and Keil board, the PHY address is
	 0x0001, on Embedded Artists board, the PHY address is 0x0000. 
	 See definition in mac.h */
#if (KEIL_BOARD_LPC23XX || ENG_BOARD_LPC24XX)
  if ( PHYInit( NATIONAL_PHY ) == FALSE )
  {
	return ( FALSE );
  }
#else
#if (IAR_BOARD_LPC23XX || EA_BOARD_LPC24XX)
  if ( PHYInit( MICREL_PHY ) == FALSE )
  {
	return ( FALSE );
  }
#endif
#endif

  /* write the station address registers */
  MAC_SA0 = EMAC_ADDR12; 
  MAC_SA1 = EMAC_ADDR34; 
  MAC_SA2 = EMAC_ADDR56;
   
  if ( (Speed == SPEED_10) && (Duplex == HALF_DUPLEX) )
  {
	MAC_MAC2 = 0x30;		/* half duplex, CRC and PAD enabled. */
	MAC_SUPP &= ~0x0100;	/* RMII Support Reg. speed is set to 10M */
#if RMII
	MAC_COMMAND |= 0x0240;
#else
	MAC_COMMAND |= 0x0040;	/* [10]-half duplex,[9]-MII mode,[6]-Pass runt 
							frame, [5]-RxReset */
#endif
	/* back to back int-packet gap */
	MAC_IPGT = 0x0012;		/* IPG setting in half duplex mode */ 
  }
  else if ( (Speed == SPEED_100) && (Duplex == HALF_DUPLEX) )
  {
	MAC_MAC2 = 0x30;		/* half duplex, CRC and PAD enabled. */
	MAC_SUPP |= 0x0100;		/* RMII Support Reg. speed is set to 100M */
#if RMII
	MAC_COMMAND |= 0x0240;
#else
	MAC_COMMAND |= 0x0040;	/* [10]-half duplex,[9]-MII mode,[6]-Pass runt frame,
							[5]-RxReset */
#endif
	/* back to back int-packet gap */
	MAC_IPGT = 0x0012;		/* IPG setting in half duplex mode */
  }
  else if ( (Speed == SPEED_10) && (Duplex == FULL_DUPLEX) )
  {
	MAC_MAC2 = 0x31;		/* full duplex, CRC and PAD enabled. */
	MAC_SUPP &= ~0x0100;	/* RMII Support Reg. speed is set to 10M */
#if RMII
	MAC_COMMAND |= 0x0640;
#else
	MAC_COMMAND |= 0x0440;	/* [10]-full duplex,[9]-MII mode,[6]-Pass runt frame,
							[5]-RxReset */
#endif
	/* back to back int-packet gap */
	MAC_IPGT = 0x0015;		/* IPG setting in full duplex mode */
  }
  else if ( (Speed == SPEED_100) && (Duplex == FULL_DUPLEX) )			
							/* default setting, 100 BASE, FULL DUPLEX */
  {
	MAC_MAC2 = 0x31;		/* full duplex, CRC and PAD enabled. */
	MAC_SUPP |= 0x0100;		/* RMII Support Reg. speed is set to 100M */
#if RMII
	MAC_COMMAND |= 0x0640;
#else
	MAC_COMMAND |= 0x0440;	/* [10]-full duplex,[9]-MII mode,[6]-Pass runt frame,
							[5]-RxReset */
#endif
	/* back to back int-packet gap */
	MAC_IPGT = 0x0015;	/* IPG setting in full duplex mode */
  }

  EMACTxDescriptorInit();
  EMACRxDescriptorInit();

  MAC_MAC1 |= 0x0002;		/* [1]-Pass All Rx Frame */

  /* Set up RX filter, accept broadcast and perfect station */
#if ACCEPT_BROADCAST
  MAC_RXFILTERCTRL = 0x0022;	/* [1]-accept broadcast, [5]accept perfect */
#else
  MAC_RXFILTERCTRL = 0x0020;	/* accept perfect match only */  
#endif

#if MULTICAST_UNICAST
  MAC_RXFILTERCTRL |= 0x0005;
#endif

#if ENABLE_HASH
  MAC_RXFILTERCTRL |= 0x0018;
#endif

  MAC_INTCLEAR = 0xFFFF;	/* clear all MAC interrupts */    
  /* MAC interrupt related register setting */
  if ( install_irq( EMAC_INT, (void *)EMACHandler, HIGHEST_PRIORITY ) == FALSE )
  {
	return (FALSE);
  }

#if ENABLE_WOL
  MAC_RXFILTERWOLCLR = 0xFFFF;/* set all bits to clear receive filter WOLs */
  MAC_RXFILTERCTRL |= 0x2000;	/* enable Rx Magic Packet and RxFilter Enable WOL */
  MAC_INTENABLE = 0x2000;	/* only enable WOL interrupt */
#else
  MAC_INTENABLE = 0x00FF;	/* Enable all interrupts except SOFTINT and WOL */
#endif
  return ( TRUE );
}

/*****************************************************************************
** Function name:		EMACReceiveFractions
**
** Descriptions:		Dealing with a fraction of EMAC packet
**
** parameters:			StartIndex and End Index
** Returned value:		packet length
** 
*****************************************************************************/
DWORD EMACReceiveFractions( DWORD StartIndex, DWORD EndIndex )
{
  DWORD i, RxLength = 0;
  DWORD RxSize;
  DWORD *rx_status_addr;

  for ( i = StartIndex; i < EndIndex; i++ )
  {
	/* Get RX status, two words, status info. and status hash CRC. */
	rx_status_addr = (DWORD *)(RX_STATUS_ADDR + StartIndex * 8);
	RxSize = (*rx_status_addr & DESC_SIZE_MASK) - 1;
	/* two words at a time, packet and control */
	CurrentRxPtr += EMAC_BLOCK_SIZE;
	StartIndex++;
	/* last fragment of a frame */
	if ( *rx_status_addr & RX_DESC_STATUS_LAST ) 
	{
	  /* set INT bit and RX packet size */
	  MAC_RXCONSUMEINDEX = StartIndex;
	  RxLength += RxSize;
	  break; 
	}
	else	/* In the middle of the frame, the RxSize should be EMAC_BLOCK_SIZE */
			/* In the emac.h, the EMAC_BLOCK_SIZE has been set to the largest 
			ethernet packet length to simplify the process, so, it should not 
			come here in any case to deal with fragmentation. Otherwise, 
			fragmentation and repacking will be needed. */
	{
	  /* set INT bit and maximum block size */
	  MAC_RXCONSUMEINDEX = StartIndex;
	  /* wait until the whole block is received, size is EMAC_BLOCK_SIZE. */
	  while ( (*rx_status_addr & DESC_SIZE_MASK) != (EMAC_BLOCK_SIZE - 1));
	  RxLength += RxSize;
	}
  }
  return( RxLength );
}

/*****************************************************************************
** Function name:		EMACReceive
**
** Descriptions:		Receive a EMAC packet, called by ISR
**
** parameters:			buffer pointer
** Returned value:		packet length
** 
*****************************************************************************/
DWORD EMACReceive( DWORD *EMACBuf )
{
  DWORD RxProduceIndex, RxConsumeIndex;
  DWORD RxLength = 0;
  DWORD Counter = 0;

  /* the input parameter, EMCBuf, needs to be word aligned */
  RxProduceIndex = MAC_RXPRODUCEINDEX;
  RxConsumeIndex = MAC_RXCONSUMEINDEX;

  if ( RxProduceIndex == EMAC_RX_DESCRIPTOR_COUNT )
  {
	/* reach the limit, that probably should never happen */
	MAC_RXPRODUCEINDEX = 0;
	CurrentRxPtr = EMAC_RX_BUFFER_ADDR;
  }

  /* a packet has arrived. */
  if ( RxProduceIndex != RxConsumeIndex )
  {
	if ( RxProduceIndex < RxConsumeIndex )	/* Wrapped around already */
	{
	  /* take care of unwrapped, RxConsumeIndex to EMAC_RX_DESCERIPTOR_COUNT */
	  RxLength += EMACReceiveFractions( RxConsumeIndex, EMAC_RX_DESCRIPTOR_COUNT );
	  Counter++;
	  PacketReceived = TRUE;	    
	
	  /* then take care of wrapped, 0 to RxProduceIndex */
	  if ( RxProduceIndex > 0 )
	  {
		RxLength += EMACReceiveFractions( 0, RxProduceIndex );
		Counter++;
	  }
	}
	else					/* Normal process */
	{
	    RxLength += EMACReceiveFractions( RxConsumeIndex, RxProduceIndex );
		Counter++;	
	}
  }
  return( RxLength );
}

/*****************************************************************************
** Function name:		EMACSend
**
** Descriptions:		Send a EMAC packet
**
** parameters:			buffer pointer, buffer length
** Returned value:		true or false
** 
*****************************************************************************/
DWORD EMACSend( DWORD *EMACBuf, DWORD length )
{
  DWORD *tx_desc_addr;
  DWORD TxProduceIndex;
  DWORD TxConsumeIndex;
  DWORD i, templen;

  TxProduceIndex = MAC_TXPRODUCEINDEX;
  TxConsumeIndex = MAC_TXCONSUMEINDEX;

  if ( TxConsumeIndex != TxProduceIndex )
  {
	return ( FALSE );
  }

  if ( TxProduceIndex == EMAC_TX_DESCRIPTOR_COUNT )
  {
	/* reach the limit, that probably should never happen */
	/* To be tested */
	MAC_TXPRODUCEINDEX = 0;
  }

  if ( length > EMAC_BLOCK_SIZE )
  {
	templen = length;
	for ( i = 0; (DWORD)(length/EMAC_BLOCK_SIZE) + 1; i++ )
	{
	  templen = length - EMAC_BLOCK_SIZE;
	  /* two words at a time, packet and control */
	  tx_desc_addr = (DWORD *)(TX_DESCRIPTOR_ADDR + TxProduceIndex * 8);
	  /* descriptor status needs to be checked first */
	  if ( templen % EMAC_BLOCK_SIZE )
	  {
		/* full block */ 
		*tx_desc_addr = (DWORD)(EMACBuf + i * EMAC_BLOCK_SIZE);
		/* set TX descriptor control field */
		*(tx_desc_addr+1) = (DWORD)(EMAC_TX_DESC_INT | (EMAC_BLOCK_SIZE - 1));
		TxProduceIndex++;
		if ( TxProduceIndex == EMAC_TX_DESCRIPTOR_COUNT )
    	{
		  TxProduceIndex = 0;
		}
		MAC_TXPRODUCEINDEX = TxProduceIndex;	/* transmit now */
	  }
	  else
	  {
		/* last fragment */
		*tx_desc_addr = (DWORD)(EMACBuf + i * EMAC_BLOCK_SIZE);
		/* set TX descriptor control field */
		*(tx_desc_addr+1) = (DWORD)(EMAC_TX_DESC_INT | EMAC_TX_DESC_LAST | (templen -1) );
		TxProduceIndex++;		/* transmit now */
		if ( TxProduceIndex == EMAC_TX_DESCRIPTOR_COUNT )
    	{
		  TxProduceIndex = 0;
		}
		MAC_TXPRODUCEINDEX = TxProduceIndex;	/* transmit now */
		break;
	  }    
	}
  }
  else
  {
	tx_desc_addr = (DWORD *)(TX_DESCRIPTOR_ADDR + TxProduceIndex * 8);
	/* descriptor status needs to be checked first */
	*tx_desc_addr = (DWORD)(EMACBuf);
	/* set TX descriptor control field */
	*(tx_desc_addr+1) = (DWORD)(EMAC_TX_DESC_INT | EMAC_TX_DESC_LAST | (length -1));
	TxProduceIndex++;		/* transmit now */
	if ( TxProduceIndex == EMAC_TX_DESCRIPTOR_COUNT )
	{
	  TxProduceIndex = 0;
	}
	MAC_TXPRODUCEINDEX = TxProduceIndex;
  }
  return ( TRUE );
}




/*********************************************************************************
**                            End Of File
*********************************************************************************/
