/*
 * code.h
 *
 *  Created on: May 23, 2012
 *
 */

#ifndef CODE_H_
#define CODE_H_

#include "type.h"

#define CODE_LEN                 4

typedef struct {
	BYTE value[CODE_LEN];
	BYTE valid;
	// time expires?
} Code;

BYTE compareCode(Code*, BYTE*);

void setValid(Code*);
void setInvalid(Code*);
void setCode(Code*, BYTE*);

#endif /* CODE_H_ */
