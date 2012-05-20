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

enum {
	IDLE, PROMPT, PHOTO, AUTH_PHOTO, AUTH_CODE, OPEN_DOOR, ERROR
} states;

struct state_object {
	unsigned int state;
} so;

void update_state(unsigned int new_state) {
	so.state = new_state;
}

void init_robolock() {
	so.state = IDLE;
}

void robolock() {

	switch (so.state) {

	case IDLE:
		// clear LCD
		// turn off backlight
		// waiting for data on ADC or a key press

		while (1) {
			if ( 1/*keypress || knock*/) {
				update_state(PROMPT);
				break;
			}
		}

	case PROMPT:

		//turn on backlight

		// set_timer()

		// print to LCD
		// # to enter code
		// * to take photo

		while ( 1 /* !timeout() */) {

			if (1 /*user_entry == '*' */) {
				update_state(AUTH_CODE);
				break;
			} else if ( 1/*user_entry == '#' */) {
				update_state(PHOTO);
				break;
			}
		}

		update_state(ERROR);
		break;

	case PHOTO:

		// print LCD countdown
		// 3.. 2.. 1..

		if ( 1 /*SUCCESS == take_photo()*/) {
			update_state(AUTH_PHOTO);
		} else {
			update_state(ERROR);
		}

		break;

	case AUTH_PHOTO:

		// set timeout

		while ( 1 /*!timeout()*/) {
			if ( 1/*permission_granted()*/) {
				update_state(OPEN_DOOR);
				break;
			}
		}

		update_state(ERROR);

		break;

	case AUTH_CODE:

		// set timeout

		while ( 1/* !timeout() */) {
			if ( 1/*valid_code(user_entry()) */) {
				update_state(OPEN_DOOR);
				break;
			}
		}

		update_state(ERROR);

		break;

	case OPEN_DOOR:

		// set timeout

		// print welcome message to LCD
		while ( 1/*!timeout() */)
			;

		update_state(IDLE);

		break;

	case ERROR:

		// set timeout

		// print error message

		while (1 /*!timeout() */)
			;
		update_state(IDLE);

		break;

	}

	//	// muahahahahaha
	//
	//	//wait for knock
	//	lcdInit();
	//	lcdDisplay("waiting         ","for keypad       ");
	//	keypadVerify();
	//	busyWait(40);
	////	while(1){
	////		if(peizoListen()==1);
	////			break;
	////	}
	//
	//	//Activate  LCD Message
	//	lcdBacklight();
	//			lcdDisplay("Hello           ","                ");
	//			busyWait(200);
	//			lcdDisplay("Please          ","Face Camera     ");
	//			busyWait(200);
	//
	//	//Take Picture
	////			cameraTake();
	//			busyWait(200);
	//
	//    //Send Picture to server
	////			ethernetSend();
	//
	//	//wait for response
	////			while(1){
	////				if(networkFlag)
	////					break;
	////			}
	//
	//	//Handle response
	////			if(networkResponse== VALID_USER){
	//			lcdClear();
	//				lcdDisplay("Acess           ","Granted         ");
	//				strikeOpen();
	//				busyWait(200);
	//				strikeClose();
	//				busyWait(200);
	//				lcdBacklightOff();
	////			}

}
