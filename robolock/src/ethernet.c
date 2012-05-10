//#include "uip.h"
//#include "uip_arp.h"
//#include "network-device.h"
//#include "robolock_app.h"
//#include "clock-arch.h"
////TODO /*#include "timer.h" */
//
///*---------------------------------------------------------------------------*/
//int ethernet(void) {
//  int i;
//  uip_ipaddr_t ipaddr;
//  struct timer periodic_timer;
//
//  timer_set(&periodic_timer, CLOCK_SECOND / 2);
//
//  network_device_init();
//  uip_init();
//
//  uip_ipaddr(ipaddr, 192,168,0,2);
//  uip_sethostaddr(ipaddr);
//
//  robolock_app_init();
//
//  while(1) {
//    uip_len = network_device_read();
//    if(uip_len > 0) {
//      uip_input();
//      /* If the above function invocation resulted in data that
//	 should be sent out on the network, the global variable
//	 uip_len is set to a value > 0. */
//      if(uip_len > 0) {
//	network_device_send();
//      }
//    } else if(timer_expired(&periodic_timer)) {
//      timer_reset(&periodic_timer);
//      for(i = 0; i < UIP_CONNS; i++) {
//	uip_periodic(i);
//	/* If the above function invocation resulted in data that
//	   should be sent out on the network, the global variable
//	   uip_len is set to a value > 0. */
//	if(uip_len > 0) {
//	  network_device_send();
//	}
//      }
//
//    }
//  }
//  return 0;
//}
///*---------------------------------------------------------------------------*/


/*****************************************************************************
 *   emactest.c:  main C entry file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2005.10.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include <string.h>
#include "LPC23xx.h"                        /* LPC23xx definitions */
#include "type.h"
#include "irq.h"
#include "target.h"
#include "crc32.h"
#include "network-device.h"
#include "ethernet.h"
#include "LED.h"
#include "common.h"
#define TX_PACKET_SIZE		114

extern void Reset_Handler( void );
extern volatile DWORD ReceiveLength;
extern volatile DWORD PacketReceived;

#if ENABLE_WOL
extern volatile DWORD WOLArrived;
#endif

/******************************************************************************
** Function name:		PacketGen
**
** Descriptions:		Create a perfect packet for TX
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void PacketGen( BYTE *txptr )
{
  int i;
  DWORD crcValue;
  DWORD BodyLength = TX_PACKET_SIZE - 14;

  /* Dest address */
  *(txptr+0) = EMAC_DST_ADDR56 & 0xFF;
  *(txptr+1) = (EMAC_DST_ADDR56 >> 0x08) & 0xFF;
  *(txptr+2) = EMAC_DST_ADDR34 & 0xFF;
  *(txptr+3) = (EMAC_DST_ADDR34 >> 0x08) & 0xFF;
  *(txptr+4) = EMAC_DST_ADDR12 & 0xFF;
  *(txptr+5) = (EMAC_DST_ADDR12 >> 0x08) & 0xFF;

  /* Src address */
  *(txptr+6) = EMAC_ADDR56 & 0xFF;
  *(txptr+7) = (EMAC_ADDR56 >> 0x08) & 0xFF;
  *(txptr+8) = EMAC_ADDR34 & 0xFF;
  *(txptr+9) = (EMAC_ADDR34 >> 0x08) & 0xFF;
  *(txptr+10) = EMAC_ADDR12 & 0xFF;
  *(txptr+11) = (EMAC_ADDR12 >> 0x08) & 0xFF;

  /* Type or length, body length is TX_PACKET_SIZE - 14 bytes */
  *(txptr+12) = BodyLength & 0xFF;
  *(txptr+13) = (BodyLength >> 0x08) & 0xFF;

  char *hw = "hello world ";

  /* Skip the first 14 bytes for dst, src, and type/length */
  for ( i=0; i < BodyLength; i++ )
  {
	*(txptr+i+14) = hw[i%12];
  }
  crcValue = crc32_bfr( txptr, TX_PACKET_SIZE );

  *(txptr+TX_PACKET_SIZE) = (0xff & crcValue);
  *(txptr+TX_PACKET_SIZE+1) = 0xff & (crcValue >> 8 );
  *(txptr+TX_PACKET_SIZE+2) = 0xff & (crcValue >> 16);
  *(txptr+TX_PACKET_SIZE+3) = 0xff & (crcValue >> 24);
  return;
}

/*****************************************************************************
** Function name:		LED_Blink
**
** Descriptions:		Based on the pattern, display accordingly.
**						This is used for WOL test only.
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void LED_Blink( DWORD pattern )
{
  DWORD i, j;

  FIO2DIR = pattern;
  for ( i = 0; i < 0x05; i++ )
  {
	FIO2SET = pattern;
	for ( j = 0; j < 0x400000; j++ );
	FIO2CLR = pattern;
	for ( j = 0; j < 0x400000; j++ );
  }
  return;
}

/*****************************************************************************
** Function name:		AllPacketGen
**
** Descriptions:		Fill all the TX buffers based on the number
**						of TX blocks.
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void AllPacketGen ( void )
{
  DWORD i;
  BYTE *txptr;

  txptr = (BYTE *)EMAC_TX_BUFFER_ADDR;
  for ( i = 0; i < EMAC_TX_BLOCK_NUM; i++ )
  {
	PacketGen( txptr );
	txptr += EMAC_BLOCK_SIZE;
  }
  return;
}

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int ethernet (void) {
  BYTE *txptr;
  DWORD i = 0;
#if TX_ONLY
  DWORD j;
#endif
#if BOUNCE_RX
  BYTE *rxptr;
#endif
#if ENABLE_HASH
  long long dstAddr;
  DWORD gHashCrcValue;
#endif
#if ENABLE_WOL
  DWORD FirstTime_WOL = TRUE;
#endif

  /********* The main Function is an endless loop ***********/
  /* On EA and IAR boards, Micrel PHY is used.
	 on ENG and KEIL boards, the National PHY is used. */
  if ( EMACInit() == FALSE )
  {
	while ( 1 ) {
		printLED(128);
		busyWait(4);
		/* Fatal error */
		printLED(0);
		busyWait(4);

	}
  } else {
	  printLED(1);
	  busyWait(10);
  }

#if TX_ONLY
  /* pre-format the transmit packets */
  AllPacketGen();
  printLED(2);
  busyWait(10);

#endif

#if ENABLE_HASH
  dstAddr = 0x010203040506;
  gHashCrcValue = do_crc_behav( dstAddr );
  Set_HashValue( gHashCrcValue );
#endif

  EMAC_RxEnable();
  EMAC_TxEnable();

#if BOUNCE_RX
  /* copy just received data from RX buffer to TX buffer and send out */
  txptr = (BYTE *)EMAC_TX_BUFFER_ADDR;
  rxptr = (BYTE *)EMAC_RX_BUFFER_ADDR;

#if ENABLE_WOL
  INTWAKE = 0x10;			/* Ethernet(WOL) Wakeup from powerdown mode */
  LED_Blink( 0x000000F0 );	/* Indicating system is in power down now. */
  PCON = 0x02;				/* Power down first */
#endif						/* endif ENABLE_WOL */

  while( 1 )
  {
#if ENABLE_WOL
	if ( (WOLArrived == TRUE) && (FirstTime_WOL == TRUE) )	/* WOL interrupt has occured */
	{
	  WOLArrived = FALSE;
	  FirstTime_WOL = FALSE;
	  EMAC_RxDisable();
	  EMAC_TxDisable();
	  /* From power down to WOL, the PLL needs to be reconfigured,
	  otherwise, the CCLK will be generated from 4Mhz IRC instead
	  of main OSC 12Mhz */
	  ConfigurePLL();
	  LED_Blink( 0x0000000F );		/* indicating system is awake now. */

	  /* Calling EMACInit() is overkill which also initializes the PHY, the
	  main reason to do that is to make sure the descriptors and descriptor
	  status for both TX and RX are clean and ready to use. It won't go wrong. */
	  EMACInit();
	  MAC_RXFILTERWOLCLR = 0xFFFF;	/* set all bits to clear receive filter WOLs */
      MAC_RXFILTERCTRL &= ~0x2000;	/* enable Rx Magic Packet and RxFilter Enable WOL */
	  MAC_INTENABLE = 0x00FF;			/* Enable all interrupts except SOFTINT and WOL */
	  EMAC_RxEnable();
	  EMAC_TxEnable();
	  PacketReceived = FALSE;
	}
#endif										/* endif ENABLE_WOL */

	if ( PacketReceived == TRUE )
	{
	  PacketReceived = FALSE;
	  /* Reverse Source and Destination, then copy the body */
	  memcpy( (BYTE *)txptr, (BYTE *)(rxptr+6), 6);
	  memcpy( (BYTE *)(txptr+6), (BYTE *)rxptr, 6);
	  memcpy( (BYTE *)(txptr+12), (BYTE *)(rxptr+12), ReceiveLength );
	  EMACSend( (DWORD *)txptr, ReceiveLength - 2 );
	  txptr += EMAC_BLOCK_SIZE;
	  rxptr += EMAC_BLOCK_SIZE;
	  i++;
	  /* EMAC_TX_BLOCK_NUM and EMAC_RX_BLOCK_NUM should be the same */
	  if ( i == EMAC_TX_BLOCK_NUM )
	  {
		i = 0;
		txptr = (BYTE *)EMAC_TX_BUFFER_ADDR;
		rxptr = (BYTE *)EMAC_RX_BUFFER_ADDR;
	  }
	}
  }
#endif										/* endif BOUNCE_RX */

#if TX_ONLY

  printLED(4);
  busyWait(10);


  /* Transmit packets only */
  while ( 1 )
  {

	  printLED(4);
	  busyWait(10);
	  printLED(8);
	  busyWait(10);

	txptr = (BYTE *)EMAC_TX_BUFFER_ADDR;
	for ( i = 0; i < EMAC_TX_BLOCK_NUM; i++ )
	{
		/* Including 4 bytes of checksum, TX_PACKET_SIZE include
	  12 bytes of SRC and DST, and 2 bytes length/type. */
	  EMACSend( (DWORD *)txptr, TX_PACKET_SIZE + 4 );


	  txptr += EMAC_BLOCK_SIZE;

	  for ( j = 0; j < 0x200000; j++ );	/* delay */
	  printLED(32);
	  busyWait(10);
	}

	  printLED(16);
	  busyWait(10);


  }
#endif										/* endif TX_ONLY */
  return 0;
}

/*****************************************************************************
**                            End Of File
*****************************************************************************/

