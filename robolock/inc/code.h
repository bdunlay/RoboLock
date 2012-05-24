/*
 * code.h
 *
 *  Created on: May 23, 2012
 *
 */

#ifndef CODE_H_
#define CODE_H_

#include "type.h"
#include "rtc.h"
#include "irq.h"

#define CODE_LEN                 4
#define MAX_CODES                16

enum expire_times {
	NO_EXPIRE,
	EXPIRE_ONE_MINUTE,
	EXPIRE_ONE_DAY,
	EXPIRE_THREE_DAYS,
	EXPIRE_ONE_WEEK,
	EXPIRE_THIRTY_DAYS
};

typedef struct {
	BYTE value[CODE_LEN];
	BYTE valid;
	BYTE expires;
	RTCTime expTime;
} Code;

/* Setters, getters */
void setValid(Code*);
void setInvalid(Code*);
void setCode(Code*, BYTE*);
void setExpireable(Code*);
void setNotExpireable(Code*);
void setExpireTime(Code*, BYTE);

/* Higher-level methods */
void resetCodes(void);
BYTE addNewCode(BYTE*, BYTE);
BYTE invalidateOldCode(BYTE*);
void invalidateExpiredCode(void);
BYTE compareCode(Code*, BYTE*);

/* RTC methods */
void RTCHandler(void) __irq;
void updateAlarmTime(void);

Code codeList[MAX_CODES];

#endif /* CODE_H_ */
