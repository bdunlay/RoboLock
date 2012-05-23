/* RoboLock
 * robolock.h
 *
 * Main robolock program
 *
 * v1.0 - 4/22/2012
 */

#ifndef __ROBOLOCK_H
#define __ROBOLOCK_H

/* Constants */

enum {
	IDLE, PROMPT, PHOTO, AUTH_PHOTO, AUTH_CODE, OPEN_DOOR, ERROR
} states;

#define MAX_CODES                16
#define CODE_LEN                 4

#define PROMPT_TIMEOUT_LEN       30

#define PROMPT_TEXT_1            "# to enter code "
#define PROMPT_TEXT_2            "* to take photo "

#define CHEESE_TEXT_1            " Taking picture "
#define CHEESE_TEXT_2            "...3............"
#define CHEESE_TEXT_3            ".......2........"
#define CHEESE_TEXT_4            "...........1...."

#define WELCOME_TEXT_1           "    Welcome!    "

#define ERROR_TEXT_1             "     ERROR      "

#define BLANK_TEXT               "                "

/* Structs */

struct state_object {
	unsigned int state;
	unsigned int permission;

} so;

/* Functions */
#include "irq.h"

void init_robolock(void);
void robolock(void);
void update_state(unsigned int);
unsigned int permission_granted(void);

void init_network(void);
void periodic_network(void);

void promptTimeoutHandler(void) __irq;
BYTE codeMatches(BYTE*);

void sayCheese(void);

/* Variables */

volatile DWORD adcValue;

volatile DWORD knockThresh;

volatile BYTE promptTimedout;
volatile BYTE promptTimeoutCount;

volatile BYTE validCodes[MAX_CODES][CODE_LEN+1]; // format of each code is [x][x][x][x][valid/invalid]

#endif
