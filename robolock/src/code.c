/*
 * code.c
 *
 *  Created on: May 23, 2012
 *
 *
 */

#include "code.h"
#include "LPC23xx.h"
#include "uart.h"
#include "common.h"

/*
 * compareCode
 * takes a possible code string "b" and tests if "a" is valid and matches the BYTE string
 * parameters:
 *   a: pointer to an existing code struct (can be valid or invalid)
 *   b: array of BYTEs to compare code values
 *
 * returns:
 *   TRUE if a valid code matches
 *   FALSE otherwise
 *
 */
BYTE compareCode(Code* a, BYTE* b)
{
	BYTE match, i;
	match = a->valid; 								// check if code is valid
	if (!match) return FALSE; 						// if invalid, then FALSE
	for (i=0; i<CODE_LEN; i++) {
		match &= (b[i] == a->value[i]); 			// if test[j] = validCode[j], then stay true
	}
	if (match) return TRUE;
	else return FALSE;
}


/*
 * codeMatches
 *
 */
BYTE codeMatches(BYTE* toTest) {
	WORD i;
	BYTE match;
	for (i=0; i<MAX_CODES; i++) {
		match = compareCode(&codeList[i], toTest);
		if (match) return TRUE;  						// if match = TRUE after iterating through a code, return TRUE
	}
	return FALSE;
}

/*
 * resetCodes
 * initializes all codes to zeros and sets them to invalid
 * parameters:
 *   none
 *
 * returns:
 *   nothing
 */

void resetCodes()
{
	WORD i;
	BYTE zeros[CODE_LEN];
	for (i=0; i<CODE_LEN; i++)
		zeros[i] = 0;
	for (i=0; i<MAX_CODES; i++)
	{
		setCode(&codeList[i], zeros);
		setInvalid(&codeList[i]);
		setNotExpireable(&codeList[i]);
	}
}

/*
 * addNewCode
 * Takes a BYTE array, adds it to the code list, and makes it valid
 * If it expires, set an expiration date, and update the alarm if necessary
 * parameters:
 *  a: BYTE array with new code string
 *  b: enum with expiration time
 *
 * returns:
 *   TRUE if no code has been overwritten
 *   FALSE if the maximum number of codes has been reached.  NOTE: the new code will overwrite the last code in the array.
 */

BYTE addNewCode(BYTE* a, BYTE expire)
{
	WORD i;
	for (i=0; i<MAX_CODES; i++)
	{
		if (!(codeList[i].valid))
		{
			setCode(&codeList[i], a);
			setValid(&codeList[i]);
			if (expire != NO_EXPIRE)
			{
				setExpireable(&codeList[i]);
				setExpireTime(&codeList[i], expire);
				updateAlarmTime();
			}
			else setNotExpireable(&codeList[i]);
			return TRUE;
		}
	}
	setCode(&codeList[MAX_CODES-1], a);
	setValid(&codeList[i]);
	if (expire != NO_EXPIRE)
	{
		setExpireable(&codeList[i]);
		setExpireTime(&codeList[i], expire);
		updateAlarmTime();
	}
	else setNotExpireable(&codeList[i]);
	return FALSE;
}

/*
 * invalidateCode
 * takes a BYTE array, if it exists as a valid code in the array, then invalidate it
 * parameters:
 *   a: BYTE array with code to be invalidated
 *
 * returns:
 *   TRUE if the code was successfully invalidated
 *   FALSE if the code didn't exist
 */

BYTE invalidateOldCode(BYTE* a)
{
	WORD i;
	for (i=0; i<MAX_CODES; i++)
	{
		if (compareCode(&codeList[i], a))
		{
			setInvalid(&codeList[i]);
			if (codeList[i].expires)		// if the code would have expired
				updateAlarmTime();			// check to make sure that the alarm time hasn't changed
			return TRUE;
		}
	}
	return FALSE;
}

/*
 * RTCHandler
 * interrupt handler that invalidates expired codes and changes the
 * new alarm time to the nearest future expiration time
 * parameters:
 *   none
 *
 * returns:
 *   nothing
 */

void RTCHandler(void)
{
	RTC_ILR |= ILR_RTCCIF;		/* clear interrupt flag */
	IENABLE;			/* handles nested interrupt */

	UARTSendChar('R');
	invalidateExpiredCode();
	updateAlarmTime();
	UARTSendChar('r');

	IDISABLE;
	VICVectAddr = 0;		/* Acknowledge Interrupt */
}

/*
 * updateAlarmTime
 * changes the RTC alarm time to the nearest future expiration time
 * from the list of valid codes
 */

void updateAlarmTime(void)
{
	WORD i;
	RTCTime closest;
	closest.RTC_Sec = 0;
	closest.RTC_Min = 0;
	closest.RTC_Hour = 0;
	closest.RTC_Mday = 0;
	closest.RTC_Wday = 0;
	closest.RTC_Yday = 0;
	closest.RTC_Mon = 0;
	closest.RTC_Year = 4095;
	for (i=0; i<MAX_CODES; i++)
	{
		if (codeList[i].valid && codeList[i].expires)					// if it's a valid, expiring code
		{
			if (compareTime(&closest, &(codeList[i].expTime)) < 0) 		// if the valid, not-expired code is earlier than the "closest" time
			{
				closest = codeList[i].expTime;
				UARTSendChar('u');
			}
		}
	}
	RTCSetAlarm(closest);
}

/*
 * invalidateExpiredCode
 * checks through all valid codes and sets expired codes to invalid
 * parameters:
 *   none
 *
 * returns:
 *   nothing
 */

void invalidateExpiredCode(void)
{
	WORD i;
	RTCTime currTime = RTCGetTime();
	for (i=0; i<MAX_CODES; i++)
	{
		if (codeList[i].valid && codeList[i].expires)					// if it's a valid, expiring code
		{
			if (compareTime(&currTime, &(codeList[i].expTime)) < 0) 	// if the current time is later than the expiration time
			{
				setInvalid(&codeList[i]);
				UARTSendChar('e');
			}
		}
	}
}

void setValid(Code* a)
{
	a->valid = TRUE;
}

void setInvalid(Code* a)
{
	a->valid = FALSE;
}

void setCode(Code* a, BYTE* b)
{
	BYTE i;
	for (i=0; i<CODE_LEN; i++)
		a->value[i] = b[i];
}

void setExpireable(Code* a)
{
	a->expires = TRUE;
}

void setNotExpireable(Code* a)
{
	a->expires = FALSE;
}

// NOTE: does not take into account leap years
void setExpireTime(Code* a, BYTE expire)
{
	RTCTime currTime = RTCGetTime();
	switch (expire) {
	case EXPIRE_ONE_MINUTE:
		currTime.RTC_Min++;
		if (currTime.RTC_Min > 60) {
			currTime.RTC_Min = 0;
			currTime.RTC_Hour++;
			if (currTime.RTC_Hour > 24) {
				currTime.RTC_Hour = 0;
				currTime.RTC_Yday++;
				if (currTime.RTC_Yday > 365) {
					currTime.RTC_Yday = 0;
					currTime.RTC_Year++;
				}
			}
		}
		break;
	case EXPIRE_ONE_DAY:
		currTime.RTC_Yday += 1;
		if (currTime.RTC_Yday > 365) {
			currTime.RTC_Yday = 365 - (currTime.RTC_Yday);
			currTime.RTC_Year++;
		}
		break;
	case EXPIRE_THREE_DAYS:
		currTime.RTC_Yday += 3;
		if (currTime.RTC_Yday > 365) {
			currTime.RTC_Yday = 365 - (currTime.RTC_Yday);
			currTime.RTC_Year++;
		}
		break;
	case EXPIRE_ONE_WEEK:
		currTime.RTC_Yday += 7;
		if (currTime.RTC_Yday > 365) {
			currTime.RTC_Yday = 365 - (currTime.RTC_Yday);
			currTime.RTC_Year++;
		}
		break;
	case EXPIRE_THIRTY_DAYS:
		currTime.RTC_Yday += 30;
		if (currTime.RTC_Yday > 365) {
			currTime.RTC_Yday = 365 - (currTime.RTC_Yday);
			currTime.RTC_Year++;
		}
		break;
	default:
		currTime.RTC_Year = 4000;
		break;
	}
	a->expTime = currTime;
}

void testCodes()
{
	BYTE fives[CODE_LEN];
	BYTE sixes[CODE_LEN];
	BYTE incr[CODE_LEN];
	WORD i;
	for (i=0; i<CODE_LEN; i++)
	{
		fives[i] = 5;
		sixes[i] = 6;
		incr[i] = i;
	}
	resetCodes();

	addNewCode(fives, EXPIRE_ONE_MINUTE);
	addNewCode(sixes, NO_EXPIRE);
	busyWait(30000);
	UARTSendHexWord((WORD)codeMatches(fives)); // TRUE
	UARTSendHexWord((WORD)codeMatches(sixes)); // TRUE
	busyWait(30000);
	UARTSendHexWord((WORD)codeMatches(fives)); // FALSE
	UARTSendHexWord((WORD)codeMatches(sixes)); // TRUE

	addNewCode(incr, EXPIRE_ONE_MINUTE);
	busyWait(30000);
	addNewCode(fives, EXPIRE_ONE_MINUTE);
	UARTSendHexWord((WORD)codeMatches(fives)); // TRUE
	UARTSendHexWord((WORD)codeMatches(incr));  // TRUE
	busyWait(30000);
	UARTSendHexWord((WORD)codeMatches(fives)); // TRUE
	UARTSendHexWord((WORD)codeMatches(incr));  // FALSE
	busyWait(30000);
	UARTSendHexWord((WORD)codeMatches(fives)); // FALSE
	UARTSendHexWord((WORD)codeMatches(incr));  // FALSE

}
