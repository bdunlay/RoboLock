/*
 * code.c
 *
 *  Created on: May 23, 2012
 *
 *
 */

#include "code.h"

BYTE compareCode(Code* a, BYTE* b)
{
	BYTE match, i;
	match = a->valid; 								// check if code is valid
	if (!match) return FALSE; 						// if invalid, then skip
	for (i=0; i<CODE_LEN; i++) {
		match &= (b[i] == a->value[i]); 			// if test[j] = validCode[j], then stay true
	}
	if (match) return TRUE;
	else return FALSE;
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
