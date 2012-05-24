/*****************************************************************************
 *   rtc.h:  Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.13  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __RTC_H 
#define __RTC_H

#include "irq.h"
#include "type.h"

typedef struct {
    BYTE RTC_Sec;     /* Second value - [0,59] */
    BYTE RTC_Min;     /* Minute value - [0,59] */
    BYTE RTC_Hour;    /* Hour value - [0,23] */
    BYTE RTC_Mday;    /* Day of the month value - [1,31] */
    BYTE RTC_Mon;     /* Month value - [1,12] */
    BYTE RTC_Wday;    /* Day of week value - [0,6] */
    WORD RTC_Yday;    /* Day of year value - [1,365] */
    WORD RTC_Year;    /* Year value - [0,4095] */
} RTCTime;

#define IMSEC		0x00000001
#define IMMIN		0x00000002
#define IMHOUR		0x00000004
#define IMDOM		0x00000008
#define IMDOW		0x00000010
#define IMDOY		0x00000020
#define IMMON		0x00000040
#define IMYEAR		0x00000080

#define AMRSEC		0x00000001  /* Alarm mask for Seconds */
#define AMRMIN		0x00000002  /* Alarm mask for Minutes */
#define AMRHOUR		0x00000004  /* Alarm mask for Hours */
#define AMRDOM		0x00000008  /* Alarm mask for Day of Month */
#define AMRDOW		0x00000010  /* Alarm mask for Day of Week */
#define AMRDOY		0x00000020  /* Alarm mask for Day of Year */
#define AMRMON		0x00000040  /* Alarm mask for Month */
#define AMRYEAR		0x00000080  /* Alarm mask for Year */

#define PREINT_RTC	0x000001C8  /* Prescaler value, integer portion, 
				    PCLK = 15Mhz */
#define PREFRAC_RTC	0x000061C0  /* Prescaler value, fraction portion, 
				    PCLK = 15Mhz */
#define ILR_RTCCIF	0x01
#define ILR_RTCALF	0x02

#define CCR_CLKEN	0x01
#define CCR_CTCRST	0x02
#define CCR_CLKSRC	0x10

extern void RTCHandler (void) __irq;
extern void RTCInit( void );
extern void RTCStart( void );
extern void RTCStop( void );
extern void RTC_CTCReset( void );
extern void RTCSetTime( RTCTime );
extern RTCTime RTCGetTime( void );
extern void RTCSetAlarm( RTCTime );
extern void RTCSetAlarmMask( DWORD AlarmMask );

int compareTime(RTCTime*, RTCTime*);

#endif /* end __RTC_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
