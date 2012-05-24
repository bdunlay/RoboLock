/*
 * code.c
 *
 *  Created on: May 23, 2012
 *
 *
 */

#include "code.h"

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
	if (!match) return FALSE; 						// if invalid, then skip
	for (i=0; i<CODE_LEN; i++) {
		match &= (b[i] == a->value[i]); 			// if test[j] = validCode[j], then stay true
	}
	if (match) return TRUE;
	else return FALSE;
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
	}
}

/*
 * addNewCode
 * takes a BYTE array, adds it to the code list, and makes it valid
 * parameters:
 *  a: BYTE array with new code string
 *
 * returns:
 *   TRUE if no code has been overwritten
 *   FALSE if the maximum number of codes has been reached.  NOTE: the new code will overwrite the last code in the array.
 */

BYTE addNewCode(BYTE* a)
{
	WORD i;
	for (i=0; i<MAX_CODES; i++)
	{
		if (!(codeList[i].valid))
		{
			setCode(&codeList[i], a);
			setValid(&codeList[i]);
			return TRUE;
		}
	}
	setCode(&codeList[MAX_CODES-1], a);
	setValid(&codeList[i]);
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
			return TRUE;
		}
	}
	return FALSE;
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
