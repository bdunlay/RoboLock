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
#include "code.h"
#include "button.h"

//#include "uip_timer.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "clock-arch.h"
#include "clock.h"
#include "tcpclient.h"
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

void robolock() {

	BYTE codeEntered[CODE_LEN];
	BYTE codeIdx;
	//BYTE last = 0;
	char displayCode[16] = "Enter Code:     ";
	DWORD savedValue;
	WORD i;

	while (1) { //do forever
//		while(1){
//			periodic_network();
//			//busyWait(1000);
//		}

		switch (so.state) {

		case IDLE:
//			lcdDisplay("      IDLE      ", "                ");
//			lcdBacklightOff(); // backlight OFF
//
//			ADC0Read(); // start reading from the piezo
			busyWait(500);
//
//			if (buttonPressed)
//			{
//				buttonPressed = FALSE;
//				update_state(CALIBRATE);
//			}
//			else if (keypadValue != 0)//  TODO:|| ADC0Value > knockThresh) // if someone pressed a key or knocked hard enough
//			{
				printLED(keypadValue);
//				keypadValue = 0; // no need?  reset the keypad value to "unpressed"
//				update_state(PROMPT);
//			}
			break;

		case PROMPT:
			lcdBacklight(); // backlight ON

			enable_timer(2); // start prompt timeout

			lcdDisplay(PROMPT_TEXT_1, PROMPT_TEXT_2);

			while (!promptTimedout) {
				savedValue = keypadValue;
				keypadValue =  0;
				if (savedValue == 0) {
					continue;
				} else if (savedValue == '#') {
					update_state(AUTH_CODE);
					break;
				} else if (savedValue == '*') {
					update_state(PHOTO);
					break;
				} else {
					reset_timer(2);
					promptTimeoutCount = 0; // reset the timeout counter if a non-recognized character is seen
				}
			}

			if (promptTimedout)
				update_state(ERROR);
			break;

		case PHOTO:
			disable_timer(2); // disable the timer while the camera takes a picture

			sayCheese(); // print LCD countdown
			cameraTake();
			update_state(SEND_PHOTO);

			//		reset_timer(2);
			//		enable_timer(2);
			//		promptTimeoutCount = 0;

			// if (!promptTimedout) update_state(AUTH_PHOTO);
			//		else
			//update_state(ERROR);

			break;

		case SEND_PHOTO:
			lcdDisplay("Sending photo...", "----------------");
			busyWait(1000);
			while (!so.photo_sent)
				;
			so.photo_sent = 0;
			update_state(AUTH_PHOTO);

			break;

		case AUTH_PHOTO:

			lcdDisplay("TOOK PHOTO!!!!!!", "SENT PHOTO!!!!!!");
			busyWait(4000);
			update_state(IDLE);

//			promptTimeoutCount = 0;  // reset timeout counter
//			reset_timer(2);
//			enable_timer(2);
//
//			while (!promptTimedout) {
//				if (so.permission) {
//					so.permission = 0;
//					update_state(OPEN_DOOR);
//					break;
//				}
//			}
//
//			if (promptTimedout) update_state(ERROR);

			break;

		case AUTH_CODE:
			promptTimeoutCount = 0; // reset timeout counter
			reset_timer(2);
			enable_timer(2);

			codeIdx = 0;									// reset code index to point at the beginning of the code array
			for (i=0; i<16; i++)
				displayCode[i] = ' ';						// clear

			lcdDisplay(ENTER_CODE_TEXT_1, displayCode);
			while (!promptTimedout) {
				savedValue = keypadValue;					// save the value in case it changes
				keypadValue = 0;							// reset digit to unread
				if (savedValue != 0) {
					if (savedValue == '*' || savedValue == '#')
						continue;							// ignore * and # characters
					displayCode[codeIdx] = savedValue;		// show the last digit entered
					codeEntered[codeIdx] = savedValue; 		// save digit
					if (codeIdx > 0)
						displayCode[codeIdx-1] = '*';		// mask the old digits with an asterisk
					lcdDisplay(ENTER_CODE_TEXT_1, displayCode);
					++codeIdx;								// move to next digit of code
					if (codeIdx >= CODE_LEN) { 				// if the # digits entered = code length
						if (codeMatches(codeEntered)) {		// if there is a code that matches
							update_state(OPEN_DOOR);
							break;
						}
						else {								// otherwise, it's a wrong code
							update_state(ERROR);
							break;
						}
					}
				}
			}
			// TODO: merge Matt's keypadVerify with this ^ code...or not?
			if (promptTimedout)
				update_state(ERROR); // the only way to break out the loop is to timeout (ERR), enter a wrong code (ERR), or enter a right code (OK)
			break;

		case OPEN_DOOR:

			lcdDisplay(WELCOME_TEXT_1, BLANK_TEXT);

			strikeOpen();
			busyWait(5000);
			strikeClose();
			keypadValue = 0;
			update_state(IDLE);

			break;

		case ERROR:
			disable_timer(2);
			reset_timer(2);
			promptTimeoutCount = 0;
			promptTimedout = FALSE; // reset timeout flag

			keypadValue = 0;

			strikeClose(); // close door, just in case

			lcdDisplay(ERROR_TEXT_1, BLANK_TEXT);
			busyWait(5000);

			update_state(IDLE);

			break;

		case CALIBRATE:
			reset_timer(2);
			enable_timer(2);
			knockThresh = 0;
			/* Acquire maximum ADC value from a knock */
			while (!promptTimedout)
			{
				savedValue = get_ADCval();
				if (savedValue > knockThresh)
					knockThresh = savedValue;
			}
			promptTimedout = FALSE; 		// reset the timeout flag
			knockThresh = knockThresh / 2;	// anything higher than 0.5 * maximum is a significant value
			update_state(IDLE);
			break;

		}
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
	BYTE defaultCode[CODE_LEN];
	/* set initial values */
	so.state = IDLE;
	promptTimedout = FALSE;
	promptTimeoutCount = 0;
	knockThresh = 512;
	keypadValue = 0x0F;
	buttonPressed = FALSE;
	/* set initial codes */
	resetCodes(); // initialize all codes to invalid
	for (i = 0; i < CODE_LEN; i++) // create a valid default code "5555"
		defaultCode[i] = 5;
	addNewCode(defaultCode, NO_EXPIRE);
	/* initialize some systems */
	init_timer(2, Fpclk, (void*) promptTimeoutHandler, TIMEROPT_INT_RST);
	cameraReset();
}

void promptTimeoutHandler() {
	T2IR = 1;
	IENABLE;

	if (promptTimeoutCount++ > PROMPT_TIMEOUT_LEN) {
		promptTimedout = TRUE;
		promptTimeoutCount = 0; // reset timeout countdown
		disable_timer(2); // disable itself
		reset_timer(2);
	}

	IDISABLE;
	VICVectAddr = 0;
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


	// Start periodic timer
	init_timer(3, Fpclk / 10, (void*) periodic_network, TIMEROPT_INT_RST);
	reset_timer(3);

	enable_timer(3);

}
BYTE x = 0;

// need to discuss this with will
void periodic_network() {
	T3IR = 1; /* clear interrupt flag */
	IENABLE; /* handles nested interrupt */

	uip_len = tapdev_read(uip_buf);
	int i;
	//
//
//
	if (uip_len > 0) { // packed received



//		printLED(0xF0);
//		busyWait(50);
//		printLED(0x0F);
//		busyWait(50);

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

	IDISABLE;
	VICVectAddr = 0; /* Acknowledge Interrupt */
}
