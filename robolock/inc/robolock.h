/* RoboLock
 * robolock.h
 *
 * Main robolock program
 *
 * v1.0 - 4/22/2012
 */

#ifndef __ROBOLOCK_H
#define __ROBOLOCK_H

/* Includes for objects */
#include "type.h"
#include "code.h"

/* Constants */

enum {
	CONFIGURE,
	DISCONNECTED,
	IDLE,
	PROMPT,
	PHOTO,
	AUTH_PHOTO,
	SEND_PHOTO,
	AUTH_CODE,
	OPEN_DOOR,
	ERROR,
	CALIBRATE
} states;


// operate robolock without a server or network connection

#define PROMPT_TIMEOUT_LEN       20

#define PROMPT_TEXT_1            "* to take photo "
#define PROMPT_TEXT_2            "# to enter code "

#define CHEESE_TEXT_1            " Taking picture "
#define CHEESE_TEXT_2            "...3............"
#define CHEESE_TEXT_3            ".......2........"
#define CHEESE_TEXT_4            "...........1...."

#define ENTER_CODE_TEXT_1        "Enter code      "

#define WELCOME_TEXT_1           "    Welcome!    "

#define ERROR_TEXT_1             "     ERROR      "

#define BLANK_TEXT               "                "

#define CAMERA_BUFF_SIZE	1200//TODO camera response buffer size (should be same as uart2 buffer size)
#define PACKET_BUFF_SIZE	1200//TODO packet buffer size should be no more than 1500 bytes

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#define WELCOME_BUFF_SIZE		 128


/* Structs */

struct state_object {
	unsigned int state;
	unsigned int permission;
	unsigned int configured;
	unsigned int connected;
	unsigned int covert;

	// photo stuff
	char prePacketBuffer[CAMERA_BUFF_SIZE];
	char dataBuffer[PACKET_BUFF_SIZE];
	unsigned int photo_taken;
	unsigned int photo_sent;
	unsigned int data_sent;

	int photo_size;
	int photo_address;
	int chunk_length;
	int send_data_flag;

} so;

/* Functions */
#include "irq.h"

void init_robolock(void);
void robolock(void);

void update_state(unsigned int);
void promptTimeoutHandler(void) __irq;

BYTE codeMatches(BYTE*);

void sayCheese(void);

BYTE permission_granted(void);

void init_network(void);
void periodic_network(void)/* __irq*/;
int formatPacket(char*, char*, int);
int genericTakePhoto(void);
void resetStateVariables(void);
int takePhoto(void);
void sendPhoto(void);



/* Variables */

volatile BYTE NETWORK_ENABLED;

volatile DWORD adcValue;

volatile DWORD knockThresh;

volatile BYTE promptTimedout;
volatile BYTE promptTimeoutCount;

volatile BYTE welcomeMsg[WELCOME_BUFF_SIZE];

#endif
