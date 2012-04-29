/*****************************************************************************
 *   i2c.h:  Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.19  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __I2C_H 
#define __I2C_H

#define BUFSIZE			0x20
#define MAX_TIMEOUT		0x00FFFFFF

#define I2CMASTER		0x01
#define I2CSLAVE		0x02

/* For more info, read Philips's SE95 datasheet */
//#define LM75_ADDR		0x90
//#define LM75_TEMP		0x00
//#define LM75_CONFIG		0x01
//#define LM75_THYST		0x02
//#define LM75_TOS		0x03

#define LCD_ADDR		0x7C
#define LCD_TEMP		0x00	//?
#define LCD_CONFIG		0x01 //0x01	//?
#define LCD_THYST		0x02	//?
#define LCD_TOS			0x03	//?

#define RD_BIT			0x01

#define I2C_IDLE			0
#define I2C_STARTED			1
#define I2C_RESTARTED		2
#define I2C_REPEATED_START	3
#define DATA_ACK			4
#define DATA_NACK			5

#define I2CONSET_I2EN		0x00000040  /* I2C Control Set Register */
#define I2CONSET_AA			0x00000004
#define I2CONSET_SI			0x00000008
#define I2CONSET_STO		0x00000010
#define I2CONSET_STA		0x00000020

#define I2CONCLR_AAC		0x00000004  /* I2C Control clear Register */
#define I2CONCLR_SIC		0x00000008
#define I2CONCLR_STAC		0x00000020
#define I2CONCLR_I2ENC		0x00000040

#define I2DAT_I2C			0x00000000  /* I2C Data Reg */
#define I2ADR_I2C			0x00000000  /* I2C Slave Address Reg */
#define I2SCLH_SCLH			0x00000080  /* I2C SCL Duty Cycle High Reg */
#define I2SCLL_SCLL			0x00000080  /* I2C SCL Duty Cycle Low Reg */

#include "type.h"
extern void I2C1MasterHandler( void );// __irq;
extern DWORD I2CInit( DWORD I2cMode );  // 0 slave 1 master
extern DWORD I2CStart( void );
extern DWORD I2CStop( void );
extern DWORD I2CEngine( void );

#endif /* end __I2C_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/
