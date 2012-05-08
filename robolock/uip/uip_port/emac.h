/*****************************************************************************
 *   emac.h:  Header file for NXP LPC230x Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.09.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __EMAC_H 
#define __EMAC_H

/* For the EMAC test, there are two ways to test:
	TX_ONLY and BOUNCE_RX flags can be set one at a time, not both.
	When TX_ONLY is set to 1, it's a TX_ONLY packet from the MCB2300
	board to the LAN. Use the traffic analyzer such as ethereal, once
	the program is running, the packets can be monitored on the traffic
	analyzer.
	When BOUNCE_RX is set to 1 (TX_ONLY needs to reset to 0), it's a 
	test to test both TX and RX, use the traffic generator/analyzer, 
	you can creat a packet with the destination address as that on the 
	MCB2300 board, use the traffic generator to send packets, as long 
	as the destination address matches, MCB2300 will reverse the source 
	and destination address and send the packets back on the network.
	ENABLE_WOL flag is used to test power down and WOL functionalty.
	BOUNCE_RX flag needs to be set to 1 when WOL is being tested. 
*/
#define TX_ONLY				1
#define BOUNCE_RX			0
#define ENABLE_WOL			0
#define ENABLE_HASH			0

#define RMII				1	/* If zero, it's a MII interface. */
#define ACCEPT_BROADCAST	0
#define MULTICAST_UNICAST	0
	 
/* This is the MAC address of MCB23xx/24xx */
#define EMAC_ADDR12		0x0000101F
#define EMAC_ADDR34		0x0000E012
#define EMAC_ADDR56		0x00001D0C

/* A pseudo destination MAC address is defined for both TX_ONLY and
BOUNCE_RX test */
#define EMAC_DST_ADDR12		0x0000E386
#define EMAC_DST_ADDR34		0x00006BDA
#define EMAC_DST_ADDR56		0x00005000

/* National DP83848 PHY related registers */

/* PHY_ADDR, by default, AD0 has pull-up, AD1~4 have pull-downs, 
so, the default address is 0x0001, except for Embedded Artists board */
#if EA_BOARD_LPC24XX
#define PHY_ADDR		(0x0000 << 8)	/* in MAC_MADR, bit 8~12 */
#else
#define PHY_ADDR		(0x0001 << 8)	/* in MAC_MADR, bit 8~12 */
#endif

/* Standard PHY regisers, among all the PHY available, all
the standard PHY register set should be very much the same.
However, all the manufacture have their own extended register
set. */
#define PHY_BMCR		0x0000
#define PHY_BMSR		0x0001
#define PHY_PHYIDR1		0x0002
#define PHY_PHYIDR2		0x0003
#define PHY_ANAR		0x0004
#define PHY_ANLPAR		0x0005
#define PHY_ANLPARNP	0x0005
#define PHY_ANER		0x0006
#define PHY_ANNPTR		0x0007

/* Extended PHY registers */
/* Below is the National PHY definition used for Keil LPC23xx 
and NXP internal engineering board. */
#define NSM_PHY_PHYSTS		0x0010
#define NSM_PHY_MICR		0x0011
#define NSM_PHY_MISR		0x0012
#define NSM_PHY_RESERVED1	0x0013
#define NSM_PHY_FCSCR		0x0014
#define NSM_PHY_RECR		0x0015
#define NSM_PHY_PCSR		0x0016
#define NSM_PHY_RBR			0x0017
#define NSM_PHY_LEDCR		0x0018
#define NSM_PHY_PHYCR		0x0019
#define NSM_PHY_10BTSCR		0x001A
#define NSM_PHY_CDCTRL1		0x001B
#define NSM_PHY_RESERVED2	0x001C
#define NSM_PHY_EDCR		0x001D

/* Below is the Micrel PHY definition used for IAR LPC23xx and 
Embedded Artists LPC24xx board. */
#define MIC_PHY_RXER_CNT			0x0015
#define MIC_PHY_INT_CTRL			0x001B
#define MIC_PHY_LINKMD_CTRL			0x001D
#define MIC_PHY_PHY_CTRL			0x001E
#define MIC_PHY_100BASE_PHY_CTRL	0x001F
 
/* BMCR setting */
#define BMCR_RESET			0x8000
#define BMCR_LOOPBACK		0x4000
#define BMCR_SPEED_100		0x2000
#define BMCR_AN				0x1000
#define BMCR_POWERDOWN		0x0800
#define BMCR_ISOLATE		0x0400
#define BMCR_RE_AN			0x0200
#define BMCR_DUPLEX			0x0100

/* BMSR setting */
#define BMSR_100BE_T4		0x8000
#define BMSR_100TX_FULL		0x4000
#define BMSR_100TX_HALF		0x2000
#define BMSR_10BE_FULL		0x1000
#define BMSR_10BE_HALF		0x0800
#define BMSR_AUTO_DONE		0x0020
#define BMSR_REMOTE_FAULT	0x0010
#define BMSR_NO_AUTO		0x0008
#define BMSR_LINK_ESTABLISHED	0x0004

#define MII_BMSR_TIMEOUT	0x1000000
/* EMAC MODULE ID	*/
#define PHILIPS_EMAC_MODULE_ID	((0x3902 << 16) | 0x2000)

/* MAC registers and parameters */
#define PCONP_EMAC_CLOCK	0x40000000

#define SPEED_100			1
#define SPEED_10			0
#define FULL_DUPLEX			1
#define HALF_DUPLEX			0

#define EMAC_RAM_ADDR		0x7FE00000
#define EMAC_RAM_SIZE		0x00004000

/* The Ethernet RAM is configured as below, the starting of EMAC_DESCRIPTOR_ADDR depends
on the EMAC_DESCRIPTOR_COUNT or the TOTAL_DESCRIPTOR_SIZE, at this point, the 
EMAC_DESCRIPTOR_COUNT for both TX and RX is set to 16:

   EMAC_RAM_ADDR	0x7FE00000
   EMAC_DMA_ADDR	0x7FE00000
   .
   .
   .
   EMAC_DMA_END		EMAC_RAM_ADDR + EMAC_RAM_SIZE - TOTAL_DESCRIPTOR_SIZE 
   TX_DESCRIPTOR_ADDR = EMAC_DESCRIPTOR_ADDR = EMAC_RAM_END(EMAC_RAM_ADDR+EMAC_RAM_SIZE) - TOTAL_DESCRIPTOR 
   TX_STATUS_ADDR = TX_DESCRIPTOR_ADDR + TX_DESCRIPTOR_SIZE
   RX_DESCRIPTOR_ADDR = TX_DESCRIPTOR_ADDR + TX_DESCRIPTOR_SIZE + TX_STATUS_SIZE
   RX_STATUS_ADDR = RX_DESCRIPTOR_ADDR + RX_STATUS_SIZE
   ( RX_STATUS_ADDR + RX_STATUS_SIZE = EMAC_RAM_END )!!!!!
   EMAX_RAM_END	0x7FE04000

   Please note that, the descriptors have to be aligned to the 32 bit boundary!!!
   Below descriptor addresses have been carefully aligned to the 32-bit boundary. 
   If not, the descriptors have to be re-aligned!!! 
 */

#define EMAC_TX_DESCRIPTOR_COUNT	0x0010
#define EMAC_RX_DESCRIPTOR_COUNT	0x0010
#define TX_DESCRIPTOR_SIZE	(EMAC_TX_DESCRIPTOR_COUNT * 8)
#define RX_DESCRIPTOR_SIZE	(EMAC_RX_DESCRIPTOR_COUNT * 8)
#define TX_STATUS_SIZE		(EMAC_TX_DESCRIPTOR_COUNT * 4)
#define RX_STATUS_SIZE		(EMAC_RX_DESCRIPTOR_COUNT * 8)
#define TOTAL_DESCRIPTOR_SIZE	(TX_DESCRIPTOR_SIZE + RX_DESCRIPTOR_SIZE + TX_STATUS_SIZE + RX_STATUS_SIZE)
#define EMAC_DESCRIPTOR_ADDR	(EMAC_RAM_ADDR + EMAC_RAM_SIZE - TOTAL_DESCRIPTOR_SIZE) 

#define TX_DESCRIPTOR_ADDR	EMAC_DESCRIPTOR_ADDR
#define TX_STATUS_ADDR		(EMAC_DESCRIPTOR_ADDR + TX_DESCRIPTOR_SIZE)
#define RX_DESCRIPTOR_ADDR	(TX_STATUS_ADDR + TX_STATUS_SIZE)
#define RX_STATUS_ADDR		(RX_DESCRIPTOR_ADDR + RX_DESCRIPTOR_SIZE)

#define EMAC_DMA_ADDR		EMAC_RAM_ADDR
#define EMAC_DMA_SIZE		EMAC_RAM_ADDR + EMAC_RAM_END - TOTAL_DESCRIPTOR_SIZE

/* For EMAC TX and RX buffer, the fixed block size is EMAC_BLOCK_SIZE, there will 
EMAC_TX_BLOCK_NUM and EMAC_RX_BLOCK_NUM blocks for TX and RX, the total RAM size used 
is EMAC_BLOCK * (EMAC_TX_BLOCK_NUM + EMAC_RX_BLOCK_NUM) = 15360, the TOTAL_DESCRIPTOR_SIZE 
is 448, the total RAM, 16384 bytes, is just big enough for buffers and descriptors, 
EMA_BUFFER_SIZE + TOTAL_DESCRIPTOR_SIZE = 16256.

Please note, 
(1) if the EMAC_DESCRIPTOR_COUNT increases, the EMAC_BUFFER_SIZE needs to be decreased 
accordingly!!!!
(2) if the TOTAL_EMAC_BLOCK_SIZE increases, the EMAC_BLOCK_NUM needs to be decreased 
accordingly as well!!! */

#define EMAC_BLOCK_SIZE			0x600
#define EMAC_TX_BLOCK_NUM		5	
#define EMAC_RX_BLOCK_NUM		5
#define TOTAL_EMAC_BLOCK_NUM	10

#define EMAC_BUFFER_SIZE	(EMAC_BLOCK_SIZE * (EMAC_TX_BLOCK_NUM + EMAC_RX_BLOCK_NUM ))
#define EMAC_TX_BUFFER_ADDR	EMAC_RAM_ADDR
#define EMAC_RX_BUFFER_ADDR	(EMAC_RAM_ADDR + EMAC_BLOCK_SIZE * EMAC_TX_BLOCK_NUM)

/* EMAC TX DMA Descriptor */
typedef struct _EMAC_TX_DESCRIPTOR {
    DWORD TXPacketAddr;		/* TX DMA Buffer Address */
    DWORD TXControl;
} EMAC_TX_DESCRIPTOR;

typedef struct _EMAC_RX_DESCRIPTOR {
    DWORD RXPacketAddr;		/* RX DMA Buffer Address */
    DWORD RXControl;
} EMAC_RX_DESCRIPTOR;

/* EMAC Descriptor TX and RX Control fields */
#define EMAC_TX_DESC_INT		0x80000000
#define EMAC_TX_DESC_LAST		0x40000000
#define EMAC_TX_DESC_CRC		0x20000000
#define EMAC_TX_DESC_PAD		0x10000000
#define EMAC_TX_DESC_HUGE		0x08000000
#define EMAC_TX_DESC_OVERRIDE	0x04000000

#define EMAC_RX_DESC_INT		0x80000000

/* EMAC Descriptor status related definition */
#define TX_DESC_STATUS_ERR		0x80000000
#define TX_DESC_STATUS_NODESC	0x40000000
#define TX_DESC_STATUS_UNDERRUN	0x20000000
#define TX_DESC_STATUS_LCOL		0x10000000
#define TX_DESC_STATUS_ECOL		0x08000000
#define TX_DESC_STATUS_EDEFER	0x04000000
#define TX_DESC_STATUS_DEFER	0x02000000
#define TX_DESC_STATUS_COLCNT	0x01E00000	/* four bits, it's a mask, not exact count */

#define RX_DESC_STATUS_ERR		0x80000000
#define RX_DESC_STATUS_LAST		0x40000000
#define RX_DESC_STATUS_NODESC	0x20000000
#define RX_DESC_STATUS_OVERRUN	0x10000000
#define RX_DESC_STATUS_ALGNERR	0x08000000
#define RX_DESC_STATUS_RNGERR	0x04000000
#define RX_DESC_STATUS_LENERR	0x02000000
#define RX_DESC_STATUS_SYMERR	0x01000000
#define RX_DESC_STATUS_CRCERR	0x00800000
#define RX_DESC_STATUS_BCAST	0x00400000
#define RX_DESC_STATUS_MCAST	0x00200000
#define RX_DESC_STATUS_FAILFLT	0x00100000
#define RX_DESC_STATUS_VLAN		0x00080000
#define RX_DESC_STATUS_CTLFRAM	0x00040000

#define DESC_SIZE_MASK			0x000007FF	/* 11 bits for both TX and RX */

/* EMAC interrupt controller related definition */
#define EMAC_INT_RXOVERRUN	0x01 << 0
#define EMAC_INT_RXERROR	0x01 << 1 
#define EMAC_INT_RXFINISHED	0x01 << 2
#define EMAC_INT_RXDONE		0x01 << 3 
#define EMAC_INT_TXUNDERRUN	0x01 << 4
#define EMAC_INT_TXERROR	0x01 << 5 
#define EMAC_INT_TXFINISHED	0x01 << 6
#define EMAC_INT_TXDONE		0x01 << 7 
#define EMAC_INT_SOFTINT	0x01 << 12
#define EMAC_INT_WOL		0x01 << 13 

#define MAX_PACKET_SIZE		0x600
#define EMAC_HEADER_LENGTH	14
				
extern DWORD PHYInit( DWORD PHYType );
extern void EMACTxDescriptorInit( void );
extern void EMACRxDescriptorInit( void );
extern void EMAC_TxEnable( void );
extern void EMAC_TxDisable( void );
extern void EMAC_RxEnable( void );
extern void EMAC_RxDisable( void );

extern DWORD EMACInit( void );
extern DWORD EMACSend( DWORD *EMACBuf, DWORD length );
extern DWORD EMACReceive( DWORD *EMACBuf );

#endif /* end __EMAC_H */
/*****************************************************************************
*                            End Of File
******************************************************************************/

