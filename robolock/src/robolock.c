/* RoboLock
 * robolock.c
 *
 * Main ROBOLOCK control loop
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
#include "uart.h"
#include "common.h"

//#include "uip_timer.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "clock-arch.h"
#include "clock.h"
#include "tcpclient.h"


void robolock() {

	BYTE codeEntered[CODE_LEN];
	BYTE codeIdx;
	BYTE savedKeyValue;
	DWORD savedADCValue;
	char displayCode[16];
	int i;

	lcdBacklight();
	lcdDisplay("    RoboLock    ", "ECE189 2011/2012");

	busyWait(2000);

	while (1) { //do forever

		switch (so.state) {

		case CONFIGURE:
			UARTprint("[DISCONNECTED]");

			while(!so.configured)
				periodic_network();

			break;

		case DISCONNECTED:
			UARTprint("[DISCONNECTED]");

			lcdDisplay("   -RoboLock-   ", "  Connecting..  ");

			while(!so.connected)
				periodic_network();

			lcdDisplay("   -RoboLock-   ", "   Connected!   ");

			update_state(IDLE );

			break;

		case IDLE:

			UARTprint("[IDLE]");
			resetStateVariables();
			busyWait(2000);

			// Clear Display
			lcdDisplay("                ", "                ");
			lcdBacklightOff(); // backlight OFF

			// These should be in the so struct
			buttonPressed = FALSE; // reset button flag
			keypadValue = 0;

			//timer
			disable_timer(2);
			reset_timer(2);
			promptTimeoutCount = 0;
			promptTimedout = FALSE; // reset timeout flag

			while (1) {

				periodic_network();
				if (so.state != IDLE) break; // in case network changes state

//				ADC0Read();

				if (buttonPressed) {
					buttonPressed = FALSE;
					update_state(CALIBRATE);
					break;
				} else if (keypadValue != 0 ) { //|| ADC0Value > knockThresh) // if someone pressed a key or knocked hard enough
					update_state(PROMPT);
					break;
				}
			}

			break;

		case PROMPT:
			UARTprint("[PROMPT]");

			lcdBacklight(); 							// backlight ON
			lcdDisplay(PROMPT_TEXT_1, PROMPT_TEXT_2);

			enable_timer(2); 							// start prompt timeout
			keypadValue = 0;							// reset the keypad value to "unpressed"

			while (!promptTimedout) {
				savedKeyValue = keypadValue;
				if (savedKeyValue == 0) {
					continue;
				} else if (savedKeyValue == '#') {
					update_state(AUTH_CODE);
					break;
				} else if (savedKeyValue == '*') {
					update_state(ERROR);
					#if NETWORK_ENABLED
					update_state(PHOTO);
					#endif
					break;
				} else {
					reset_timer(2);
					promptTimeoutCount = 0; // reset the timeout counter if a non-recognized character is seen
					keypadValue = 0;
				}
			}

			if (promptTimedout)
				update_state(ERROR);
			break;

		case PHOTO:
			UARTprint("[PHOTO]");
			disable_timer(2); // disable the timer while the camera takes a picture

			if (!so.covert)
				sayCheese(); // print LCD countdown


			if (takePhoto()) {
				update_state(SEND_PHOTO);
			} else {
				update_state(ERROR);
			}

			break;

		case SEND_PHOTO:
			UARTprint("[SEND_PHOTO]");

			if (!so.covert) {
				lcdDisplay("Sending Photo...", "  Please Wait!  ");
				UARTprint("(Covert Photo)");
			}

			sendPhoto();

			if (so.	covert) {
				update_state(IDLE);
			} else {
				update_state(AUTH_PHOTO);
			}

//			reset_timer(2);
//			enable_timer(2);
//			promptTimeoutCount = 0;
//
// 			if (!promptTimedout)
//				update_state(AUTH_PHOTO);
//			else
//				update_state(ERROR);


			break;

		case AUTH_PHOTO:
			UARTprint("[AUTH_PHOTO]");
			lcdDisplay("Upload Complete!", " Please Wait... ");

			promptTimeoutCount = 0;  // reset timeout counter
			reset_timer(2);
			enable_timer(2);

			while (!promptTimedout) {
				periodic_network();
				if (so.permission) {
					update_state(OPEN_DOOR);
					break;
				}
			}

			if (promptTimedout) update_state(ERROR);

			break;

		case AUTH_CODE:
			UARTprint("[AUTH_CODE]");

			promptTimeoutCount = 0; // reset timeout counter
			reset_timer(2);
			enable_timer(2);

			codeIdx = 0; 										// reset code index to point at the beginning of the code array
			for (i = 0; i < 16; i++)
				displayCode[i] = ' '; 							// clear
			keypadValue = 0; 									// reset digit to unread

			lcdDisplay(ENTER_CODE_TEXT_1, displayCode);
			while (!promptTimedout) {
				savedKeyValue = keypadValue; 					// save the value in case it changes
				if (savedKeyValue != 0) {
					keypadValue = 0;							// reset digit to unread
					if (savedKeyValue == '*' || savedKeyValue == '#')
						continue; 								// ignore * and # characters
					displayCode[codeIdx] = savedKeyValue; 		// show the last digit entered
					codeEntered[codeIdx] = atoi(savedKeyValue); // save digit
					UARTSendChar(savedKeyValue);
					if (codeIdx > 0)
						displayCode[codeIdx - 1] = '*'; 		// mask the old digits with an asterisk
					lcdDisplay(ENTER_CODE_TEXT_1, displayCode);
					++codeIdx; 									// move to next digit of code
					if (codeIdx >= CODE_LEN) { 					// if the # digits entered = code length
						if (codeMatches(codeEntered)) { 		// if there is a code that matches
							update_state(OPEN_DOOR);
							UARTprint("Correct Code\0");
							break;
						} else { 								// otherwise, it's a wrong code
							update_state(ERROR);
							UARTprint("Wrong Code\0");
							break;
						}
					}
				}
			}

			if (promptTimedout) {
				update_state(ERROR); // the only way to break out the loop is to timeout (ERR), enter a wrong code (ERR), or enter a right code (OK)
				UARTprint("Timed out\0");
			}
			break;

		case OPEN_DOOR:
			UARTprint("[OPEN_DOOR]");

			lcdDisplay(WELCOME_TEXT_1, BLANK_TEXT);
			strike(5000);
			update_state(IDLE);

			break;

		case ERROR:
			UARTprint("[ERROR]");

			disable_timer(2);
			reset_timer(2);
			promptTimeoutCount = 0;
			promptTimedout = FALSE; // reset timeout flag

			strike(-1); // close door, just in case

			lcdDisplay(ERROR_TEXT_1, BLANK_TEXT);
			busyWait(5000);

			update_state(IDLE);

			break;

		case CALIBRATE:
			UARTprint("[CALIBRATE]");

			reset_timer(2);
			enable_timer(2);
			knockThresh = 0;

			/* Acquire maximum ADC value from a knock */
			while (!promptTimedout) {
				savedADCValue = get_ADCval();
				if (savedADCValue > knockThresh)
					knockThresh = savedADCValue;
			}
			promptTimedout = FALSE; // reset the timeout flag
			knockThresh = knockThresh / 2; // anything higher than 0.5 * maximum is a significant value
			update_state(IDLE);
			break;

		}

	}
}

BYTE permission_granted() {
	return so.permission;
}

void update_state(unsigned int new_state) {
	so.state = new_state;
}
void init_robolock() {
	WORD i;
	BYTE defaultCode[CODE_LEN];

	/* set initial values */
	resetStateVariables();
	so.state = IDLE;
	#if NETWORK_ENABLED
	so.state = DISCONNECTED;
	#endif

	promptTimedout = FALSE;
	promptTimeoutCount = 0;
	knockThresh = 512;
	keypadValue = 0;
	buttonPressed = FALSE;

	/* set initial codes */
	resetCodes(); // initialize all codes to invalid
	for (i = 0; i < CODE_LEN; i++) // create a valid default code "5555"
		defaultCode[i] = 5;
	addNewCode(defaultCode, NO_EXPIRE);

	/* initialize some systems */
	init_timer(2, Fpclk, (void*) promptTimeoutHandler, TIMEROPT_INT_RST);
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
}




void periodic_network() {

	uip_len = tapdev_read(uip_buf);
	int i;

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

int formatPacket(char* type, char* data, int bytes) {

	int i, k;

	for (i = 0; type[i] != '\0'; i++) {
		so.dataBuffer[i] = type[i];
	}

	so.dataBuffer[i++] = '/';

	for (k = 0; k < bytes; k++) {
		so.dataBuffer[i++] = data[k];
	}

	return i;
}

int takePhoto() {
	if (JPEGCamera_takePicture(so.prePacketBuffer)) {
		JPEGCamera_getSize(so.prePacketBuffer, &(so.photo_size));
		return 1;
	}

	return 0;
}

void sendPhoto() {

	int i = 0;
	int k = 0;
	int eof = 0;
	int count = 0;

	while (so.photo_address < so.photo_size) {

		count = JPEGCamera_readData(so.prePacketBuffer, so.photo_address);

		for (i = 5; i < count - 5; i++) {
			//Check the response for the eof indicator (0xFF, 0xD9). If we find it, set the eof flag
			if ((so.prePacketBuffer[i] == (char) 0xD9) && (so.prePacketBuffer[i - 1] == (char) 0xFF))
				eof = 1;
			k = i - 5;
			if (eof == 1)
				break;
		}
		printLED(count);
		so.photo_address += (count - 10);

		so.chunk_length = formatPacket("photo\0", so.prePacketBuffer+5, k+1);
		so.send_data_flag = 1;
		while(!so.data_sent) {
			periodic_network();
		}
		so.send_data_flag = 0;
		so.data_sent = 0;

	}

	JPEGCamera_stopPictures(so.prePacketBuffer);

	// sends the final packet to end the file
	so.chunk_length = formatPacket("photo\0", "END", 3);
	so.send_data_flag = 1;
	while(!so.data_sent) {
		periodic_network();
	}
}



void resetStateVariables() {
	so.photo_address = 0;
	so.photo_sent = 0;
	so.photo_size = 0;
	so.photo_taken = 0;
	so.send_data_flag = 0;
	so.data_sent = 0;
	so.permission = 0;
	so.covert = 0;
}
