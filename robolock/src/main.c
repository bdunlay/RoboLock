#define __MAIN_C__

#include "type.h"
#include "LPC23xx.h"
#include "target.h"
#include "common.h"
#include "led.h"
#include "dip.h"
#include "LCD.h"
#include "keypad.h"
#include "ex_sdram.h"
#include "timer.h"
#include "uart.h"
#include "adc.h"
#include "uart.h"
#include "uart3.h"
#include "ethernet.h"
#include "strike.h"
#include "robolock.h"
#define UIP_ENABLED 1

#if UIP_ENABLED

//#include "uip_timer.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "clock-arch.h"
#include "clock.h"
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#endif

enum {
	IDLE, PROMPT, PHOTO, AUTH_PHOTO, AUTH_CODE, OPEN_DOOR, ERROR
} states;

struct state_object {

	unsigned int state = IDLE;

} so;

void updateState(unsigned int new_state) {
	so->state = new_state;
}

/*****************************************************************************
 *    Main Function  main()													 *
 *****************************************************************************/

int main(void) {

	TargetResetInit();
	initLED();
	initLCD();
	init_dip();

	UARTInit(9600);
	UART3Init(9600);
	ADCInit();
	SDRAMInit();
	strikeInit();

#if UIP_ENABLED

	int i = 0;

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
	uip_ipaddr(ipaddr, 128,111,56,53);
	uip_sethostaddr(ipaddr); /* host IP address */
	uip_ipaddr(ipaddr, 128,111,56,1);
	uip_setdraddr(ipaddr); /* router IP address */
	uip_ipaddr(ipaddr, 255,255,255,0);
	uip_setnetmask(ipaddr); /* mask */

	// Initialize the HTTP server, listen to port 80.
	tcp_client_init();

#endif



	while (1) {

		switch (read_dip()) {

		case 0:

			switch (so->state) {

			case IDLE:
				// screen is off
				// waiting for data on ADC or a key press

				// if (keypress || knock)
				// update_state(PROMPT);

				break;

			case PROMPT:

				// set timer

				// if (timeout) {
				//	update_state(ERROR);
				// }

				// print to LCD
				// # to enter code
				// * to take photo

				// read keypad buffer

				// if (user_entry == '*') {
				//		update_state(AUTH_CODE);
				// } else if (user_entry == '#') {
				//		updateState(PHOTO);
				// }

				break;

			case PHOTO:

				// set timeout

				// print LCD countdown
				// 3.. 2.. 1..

				// if(SUCCESS == take_photo()) {
				// 	update_state(AUTH_PHOTO);
				// else {
				//	update_state(ERROR);
				//}

				break;

			case AUTH_PHOTO:

				// set timeout

				// if (permission_granted) {
				//	update_state(OPEN_DOOR);
				// }

				break;

			case AUTH_CODE:

				// set timeout


				// read buffer
				// if (valid_code(user_entry)) {
				//	update_state(OPEN_DOOR);
				// } else {
				//	update_state(ERROR);
				// }

				break;

			case OPEN_DOOR:

				// set timeout

				// print welcome message to LCD
				// if (timeout) {
				// 	update_state(IDLE);
				// }
				break;

			case ERROR:
				// set timeout

				// print error message
				// update_state(IDLE);

				break;

			}

			break;

			/* SUBSYSTEM TESTS */

		case 1:
			testLED();
			break;

		case 2:
			testLCD();
			break;

		case 3:
			testUART();
			break;

		case 4:
			testTimerIRQ();
			break;

		case 5:
			testStrike();
			break;

		case 6:
			testADC();
			break;

		case 7:
#if UIP_ENABLED
			/* receive packet and put in uip_buf */
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

#endif

			break;

		case 8:
			testSDRAM();
			break;

		case 9:
			testSDRAM_simple();
			break;

		case 10:
			testKeypad();
			break;

		case 11:
			break;

		case 12:
			break;

		case 13:
			break;

		case 14:
			break;

		case 15:
			break;

		default:
			break;
		}
	}

	return 0;
}

