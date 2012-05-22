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


//#include "uip_timer.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "clock-arch.h"
#include "clock.h"
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

DWORD knockThresh;

void robolock() {
	DWORD adcVal;

	switch (so.state) {

	case IDLE:
		clearLCD();
		// turn off backlight
		adcVal = get_ADCval();

		if (1/*keypress || knock*/) {
			update_state(PROMPT);
		}
		break;

	case PROMPT:

		//turn on backlight

		// set_timer()

		// print to LCD
		// # to enter code
		// * to take photo

		while (1 /* !timeout() */) {

			if (1 /*user_entry == '*' */) {
				update_state(AUTH_CODE);
				break;
			} else if (1/*user_entry == '#' */) {
				update_state(PHOTO);
				break;
			}
		}

		update_state(ERROR);
		break;

	case PHOTO:

		// print LCD countdown
		// 3.. 2.. 1..

		if (1 /*SUCCESS == take_photo()*/) {
			update_state(AUTH_PHOTO);
		} else {
			update_state(ERROR);
		}

		break;

	case AUTH_PHOTO:

		// set timeout

		while (1 /*!timeout()*/) {
			if (1/*permission_granted()*/) {
				update_state(OPEN_DOOR);
				so.permission = 0;
				break;
			}
		}

		update_state(ERROR);

		break;

	case AUTH_CODE:

		// set timeout

		while (1/* !timeout() */) {
			if (1/*valid_code(user_entry()) */) {
				update_state(OPEN_DOOR);
				break;
			}
		}

		update_state(ERROR);

		break;

	case OPEN_DOOR:

		// set timeout

		// print welcome message to LCD
		while (1/*!timeout() */)
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

unsigned int permission_granted() {
	return so.permission;
}

void update_state(unsigned int new_state) {
	so.state = new_state;
}

void init_robolock() {
	so.state = IDLE;
	knockThresh = 512;
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
