#include "tcpclient.h"
#include "uip.h"
#include "led.h"
#include "type.h"
#include "common.h"
#include "timer.h"





void generate_header() {

}




void go(void);


void tcp_client_init(void) {
go();

}

int state = 0;

void go() {
	u16_t ipaddr[2];

	uip_ipaddr(ipaddr, 128,111,56,203);
	uip_connect(ipaddr, HTONS(9090));
	printLED(255);
	busyWait(100);

}

void tcp_client_appcall(void) {
	//   if(uip_newdata() || uip_rexmit()) {
	//      uip_send("HELLO WORLD!!!!!\n", 17);
	//   }

	printLED(1);
	busyWait(100);
	if (uip_aborted()) {
		printLED(2);
		busyWait(100);
		go();
	} else if (uip_timedout()) {
		printLED(3);
		busyWait(100);
		go();
	} else if (uip_closed()) {
		printLED(4);
		busyWait(100);
		go();
	} else if (uip_connected() || uip_newdata()) {
		char buff[100];
		if (state == 0) {
			printLED(5);
			busyWait(100);
			uip_send("Hello CSIL, are you there?", 29);
		//	state = 1;
		} else if (state == 1) {
			uip_send("Great! Goodbye!", 15);
			state = 2;
		}

	}

}
