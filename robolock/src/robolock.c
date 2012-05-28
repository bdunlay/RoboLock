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
#include "uart.h"
#include "common.h"

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
	char displayCode[16];
	BYTE savedKeyValue;
	DWORD savedADCValue;
	int i, k, eof, count;

	so.state = DISCONNECTED;
	lcdBacklight();

	while (1) { //do forever

		switch (so.state) {

		case DISCONNECTED:
			lcdDisplay("Connecting...   ", "Please wait...  ");
			while(!so.connected);

			UARTprint("Connected! \0");
			update_state(IDLE);

			break;

		case IDLE:
			UARTprint("Idle \0");
			lcdDisplay("      IDLE      ", "                ");
			busyWait(2000);
			lcdBacklightOff(); // backlight OFF
			buttonPressed = FALSE; // reset button flag
			keypadValue = 0;

			disable_timer(2);
			reset_timer(2);
			promptTimeoutCount = 0;
			promptTimedout = FALSE; // reset timeout flag

			so.photo_address = 0;
			so.photo_sent = 0;
			so.photo_size = 0;
			so.photo_taken = 0;

			while (1)
			{
//				ADC0Read();
				if (buttonPressed) {
					buttonPressed = FALSE;
					update_state(CALIBRATE);
					break;
				}
				else if (keypadValue != 0 )//|| ADC0Value > knockThresh) // if someone pressed a key or knocked hard enough
				{
					update_state(PROMPT);
					break;
				}
			}

			break;

		case PROMPT:
			UARTprint("Prompt \0");
			lcdBacklight(); 							// backlight ON

			enable_timer(2); 							// start prompt timeout

			keypadValue = 0;							// reset the keypad value to "unpressed"
			lcdDisplay(PROMPT_TEXT_1, PROMPT_TEXT_2);

			while (!promptTimedout) {
				savedKeyValue = keypadValue;
				if (savedKeyValue == 0) {
					continue;
				} else if (savedKeyValue == '#') {
					update_state(AUTH_CODE);
					break;
				} else if (savedKeyValue == '*') {
					update_state(PHOTO);
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
			disable_timer(2); // disable the timer while the camera takes a picture

			sayCheese(); // print LCD countdown

			if (JPEGCamera_takePicture(so.jpegResponse)) {
				JPEGCamera_getSize(so.jpegResponse, &(so.photo_size));
				lcdDisplay("Please wait...  ", "                ");
				update_state(SEND_PHOTO);
			} else {
				update_state(ERROR);
			}


			//		reset_timer(2);
			//		enable_timer(2);
			//		promptTimeoutCount = 0;

			// if (!promptTimedout) update_state(AUTH_PHOTO);
			//		else
			//update_state(ERROR);

			break;

		case SEND_PHOTO:
			UARTprint("Sending Photo...\0");
			while (so.photo_address < so.photo_size) {
				so.send_data_flag = 0;
				so.data_sent = 0;

				count = JPEGCamera_readData(so.jpegResponse, so.photo_address);

				for (i = 5; i < count - 5; i++) {
					//Check the response for the eof indicator (0xFF, 0xD9). If we find it, set the eof flag
					if ((so.jpegResponse[i] == (char) 0xD9) && (so.jpegResponse[i - 1] == (char) 0xFF))
						eof = 1;

					k = i - 5;
					if (eof == 1)
						break;
				}
				UARTprint(" \0");
				printLED(count);
				so.photo_address += (count - 10);
				so.chunk_length = formatPacket("photo\0", so.jpegResponse+5, k+1);
				UARTSendHexWord(so.jpegResponse[5]);
				UARTprint(" : \0");

				so.send_data_flag = 1;
				while(!so.data_sent);
				so.send_data_flag = 0;
				so.data_sent = 0;
			}

			so.chunk_length = formatPacket("photo\0", "END", 3);
			so.send_data_flag = 1;
			while(!so.data_sent);
			lcdDisplay("Photo Sent!     ", "                ");
			update_state(AUTH_PHOTO);

			break;

		case AUTH_PHOTO:

			busyWait(5000);
			update_state(IDLE);
			//update_state(IDLE);

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
			UARTSendChar('C');
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
							UARTSendChar('M');
							break;
						} else { 								// otherwise, it's a wrong code
							update_state(ERROR);
							UARTSendChar('b');
							break;
						}
					}
				}
			}

			if (promptTimedout) {
				update_state(ERROR); // the only way to break out the loop is to timeout (ERR), enter a wrong code (ERR), or enter a right code (OK)
				UARTSendChar('t');
			}
			break;

		case OPEN_DOOR:

			lcdDisplay(WELCOME_TEXT_1, BLANK_TEXT);

			UARTSendChar('O');

			strikeOpen();
			busyWait(5000);
			strikeClose();

			update_state(IDLE);

			break;

		case ERROR:
			disable_timer(2);
			reset_timer(2);
			promptTimeoutCount = 0;
			promptTimedout = FALSE; // reset timeout flag

			UARTprint("Error...\0");


			strikeClose(); // close door, just in case

			lcdDisplay(ERROR_TEXT_1, BLANK_TEXT);
			busyWait(5000);

			update_state(IDLE);

			break;

		case CALIBRATE:
			reset_timer(2);
			enable_timer(2);
			UARTSendChar('A');
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
	keypadValue = 0;
	buttonPressed = FALSE;
	/* set initial codes */
	resetCodes(); // initialize all codes to invalid
	for (i = 0; i < CODE_LEN; i++) // create a valid default code "5555"
		defaultCode[i] = 5;
	addNewCode(defaultCode, NO_EXPIRE);
	/* initialize some systems */
	init_timer(2, Fpclk, (void*) promptTimeoutHandler, TIMEROPT_INT_RST);
	so.connected = 0;
	so.photo_size = 0;
	so.photo_address = 0;
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

int formatPacket(char* type, char* data, int bytes) {

	int i, k;

	for (i = 0; i < 64; i++) {
		so.dataBuffer[i] = 0;
	}

	for (i = 0; type[i] != '\0'; i++) {
		so.dataBuffer[i] = type[i];
	}

	so.dataBuffer[i++] = '/';

	for (k = 0; k < bytes; k++) {
		so.dataBuffer[i++] = data[k];
	}

	return i;
}

