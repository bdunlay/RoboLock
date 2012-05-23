/* RoboLock
 * robolock.c
 *
 * Source file for GPIO control, configuration and usage of LEDs on the RoboLock board
 *
 * v1.0 - 4/22/2012
 */

#include "robolock.h"
#include "LPC23xx.h"
#include "type.h"
#include "target.h"
#include "common.h"
#include "LCD.h"
#include "led.h"
#include "strike.h"
#include "keypad.h"
#include "timer.h"
#include "cameraB.h"
#include "adc.h"


//#include "uip_timer.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "clock-arch.h"
#include "clock.h"
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

void robolock() {
	BYTE codeEntered[CODE_LEN];
	BYTE codeIdx;

	switch (so.state) {

	case IDLE:
		clearLCD();  				// cls
		lcdBacklightOff(); 			// backlight OFF

		ADC0Read(); 				// start reading from the piezo

		if (keypadValue != -1 || ADC0Value > knockThresh) // if someone pressed a key or knocked hard enough
		{
			keypadValue = -1; 		// reset the keypad value to "unpressed"
			update_state(PROMPT);
		}
		break;

	case PROMPT:
		lcdBacklight(); 			// backlight ON

		enable_timer(2); 			// start prompt timeout

		lcdDisplay(PROMPT_TEXT_1, PROMPT_TEXT_2);

		while (!promptTimedout) {
			if (keypadValue == -1) {
				continue;
			} else if (keypadValue == 0) { // TODO: !!!!!!! change value to * !!!!!!!
				update_state(AUTH_CODE);
				break;
			} else if (keypadValue == 1) { // TODO: !!!!!! change value to # !!!!!!
				update_state(PHOTO);
				break;
			}
			else {
				reset_timer(2);
				promptTimeoutCount = 0; // reset the timeout counter if a non-recognized character is seen
				keypadValue = -1;
			}
		}

		if (promptTimedout) update_state(ERROR);
		break;

	case PHOTO:
		disable_timer(2); 			// disable the timer while the camera takes a picture

		sayCheese();				// print LCD countdown
									// 3.. 2.. 1..
		// TODO: take photo

		if (1 /*SUCCESS == take_photo()*/) {
			update_state(AUTH_PHOTO);
		} else {
			update_state(ERROR);
		}

		break;

	case AUTH_PHOTO:
		promptTimeoutCount = 0;  // reset timeout counter
		reset_timer(2);
		enable_timer(2);

		while (!promptTimedout) {
			if (1/*permission_granted()*/) { // TODO: photo authenticated
				update_state(OPEN_DOOR);
				so.permission = 0;
				break;
			}
		}

		if (promptTimedout) update_state(ERROR);

		break;

	case AUTH_CODE:
		promptTimeoutCount = 0;  // reset timeout counter
		reset_timer(2);
		enable_timer(2);

		codeIdx = 0;			// reset code index to point at the beginning of the code array

		while (!promptTimedout) {
			if (keypadValue != -1) {
				codeEntered[codeIdx++] = keypadValue; 	// save digit
				keypadValue = -1;						// reset digit to unread
				if (codeIdx >= CODE_LEN && codeMatches(codeEntered)) {
					update_state(OPEN_DOOR);
					break;
				}
			}
		}

		if (promptTimedout) update_state(ERROR);
		break;

	case OPEN_DOOR:

		lcdDisplay(WELCOME_TEXT_1, BLANK_TEXT);

		strikeOpen();
		busyWait(5000);
		strikeClose();

		update_state(IDLE);

		break;

	case ERROR:
		disable_timer(2);
		reset_timer(2);
		promptTimeoutCount = 0;
		promptTimedout = FALSE; 		// reset timeout flag

		lcdDisplay(ERROR_TEXT_1, BLANK_TEXT);
		busyWait(5000);

		update_state(IDLE);

		break;

	}

}

unsigned int permission_granted() {
	return so.permission;
}

void update_state(unsigned int new_state) {
	so.state = new_state;
}

void init_robolock() {
	WORD i;
	/* set initial values */
	so.state = IDLE;
	promptTimedout = FALSE;
	promptTimeoutCount = 0;
	knockThresh = 512;
	keypadValue = -1;
	/* set initial codes */
	for (i=0; i<MAX_CODES; i++)			// initialize all codes to invalid
		validCodes[i][CODE_LEN] = FALSE;
	for (i=0; i<CODE_LEN; i++)			// create a valid default code "5555"
		validCodes[0][i] = 5;
	validCodes[0][CODE_LEN] = TRUE;
	/* initialize some systems */
	init_timer(2, Fpclk, (void*)promptTimeoutHandler, TIMEROPT_INT_RST);
	IENABLE;
}

void promptTimeoutHandler() {
	T2IR = 1;
	IENABLE;

	if (promptTimeoutCount++ > PROMPT_TIMEOUT_LEN)
	{
		promptTimedout = TRUE;
		promptTimeoutCount = 0; 		// reset timeout countdown
		disable_timer(2); 				// disable itself
		reset_timer(2);
	}

	IDISABLE;
	VICVectAddr = 0;
}

BYTE codeMatches(BYTE* toTest) {
	WORD i,j;
	BYTE match;
	for (i=0; i<MAX_CODES; i++) {
		match = validCodes[i][CODE_LEN]; 				// check if code is valid
		if (!match) continue; 							// if invalid, then skip
		for (j=0; j<CODE_LEN; j++) {
			match &= (toTest[j] == validCodes[i][j]); 	// if test[j] = validCode[j], then stay true
		}
		if (match) return TRUE;  						// if match = TRUE after iterating through a code, return TRUE
	}
	return FALSE;
}

void sayCheese() {
	lcdDisplay(CHEESE_TEXT_1, CHEESE_TEXT_2);
	busyWait(1000);
	lcdDisplay(CHEESE_TEXT_1, CHEESE_TEXT_3);
	busyWait(1000);
	lcdDisplay(CHEESE_TEXT_1, CHEESE_TEXT_4);
	busyWait(1000);
}

void init_network() {

	uip_ipaddr_t ipaddr; /* local IP address */
	//	struct timer periodic_timer, arp_timer;

	// clock init
	clock_init();
	// two timers for tcp/ip
	//	timer_set(&periodic_timer, CLOCK_SECOND / 2); /* 0.5s */
	//	timer_set(&arp_timer, CLOCK_SECOND * 10);	/* 10s */

	// ethernet init
	tapdev_init();

	// Initialize the uIP TCP/IP stack.
	uip_init();
	//	uip_ipaddr(ipaddr, 128,111,56,53);
	//	uip_ipaddr(ipaddr, 169,254,255,255);
	uip_ipaddr(ipaddr, 128, 111, 56, 53);
	uip_sethostaddr(ipaddr); /* host IP address */
	uip_ipaddr(ipaddr, 128, 111, 56, 1);
	uip_setdraddr(ipaddr); /* router IP address */
	uip_ipaddr(ipaddr, 255, 255, 255, 0);
	uip_setnetmask(ipaddr); /* mask */

	tcp_client_init();
}


// need to discuss this with will
void periodic_network() {

	int i;

	uip_len = tapdev_read(uip_buf);

	if (uip_len > 0) { // packed received

		if (BUF->type == htons(UIP_ETHTYPE_IP)) { // IP Packet
			uip_arp_ipin();
			uip_input();

			if (uip_len > 0) {
				uip_arp_out();
				tapdev_send(uip_buf, uip_len);
			}
		} else if (BUF->type == htons(UIP_ETHTYPE_ARP)) { // ARP Packet
			uip_arp_arpin();

			if (uip_len > 0) {
				tapdev_send(uip_buf, uip_len); /* ARP ack*/
			}
		}
	} else if (1) /* no packet but periodic_timer time out (0.5s)*/
	{
		// timer_reset(&periodic_timer);
		/* Call the ARP timer function every 10 seconds. */
		if (1) {
			//timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}

	for (i = 0; i < UIP_CONNS; i++) {
		uip_periodic(i);
		/* If the above function invocation resulted in data that
		 should be sent out on the network, the global variable
		 uip_len is set to a value > 0. */
		if (uip_len > 0) {
			uip_arp_out();
			tapdev_send(uip_buf, uip_len);
		}
	}
}
