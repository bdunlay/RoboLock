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

#define CODE_LEN                 4
#define MAX_CODES                16

typedef struct {
	BYTE value[CODE_LEN];
	BYTE valid;
	BYTE expires;
	RTCTime expTime;
} Code;

/* Private methods */
void setValid(Code*);
void setInvalid(Code*);
void setCode(Code*, BYTE*);

/* Public methods */
void resetCodes(void);
BYTE addNewCode(BYTE*);
BYTE invalidateOldCode(BYTE*);
BYTE compareCode(Code*, BYTE*);

Code codeList[MAX_CODES];

#endif /* CODE_H_ */
