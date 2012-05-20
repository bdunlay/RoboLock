/* RoboLock
 * robolock.h
 *
 * Main robolock program
 *
 * v1.0 - 4/22/2012
 */

#ifndef __ROBOLOCK_H
#define __ROBOLOCK_H


enum {
	IDLE, PROMPT, PHOTO, AUTH_PHOTO, AUTH_CODE, OPEN_DOOR, ERROR
} states;

struct state_object {
	unsigned int state;
	unsigned int permission;

} so;

void init_robolock(void);
void robolock(void);
unsigned int permission_granted(void);

void init_network(void);
void periodic_network(void);


#endif
