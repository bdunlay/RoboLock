#include "tcpclient.h"
#include "uip.h"
#include "led.h"
#include "type.h"
#include "common.h"
#include "timer.h"
#include "robolock.h"
#include <string.h>




void generate_header() {

}




void go(void);


void tcp_client_init(void) {
go();

}

int state = 0;

void go() {
	u16_t ipaddr[2];

	uip_ipaddr(ipaddr, 128,111,56,203); // server address
	uip_connect(ipaddr, HTONS(9090));	// serer port

}


void tcp_client_appcall(void) {
	int k;

	if (uip_aborted() || uip_timedout() || uip_closed()) {
		updateState(ERROR);
		uip_close(); // do i want to do this?
		for (k = 0; k < 3; k++) {
			printLED(255);
			busyWait(10);
			printLED(0);
			busyWait(10);
		}
		connect();
	} else if (uip_connected() || uip_newdata()) {

		// heartbeat to indicate that we're connected
		for (k = 0; k < 7; k++) {
			printLED(1>>k);
			printLED(10);
		}

		switch(so.state) {

			case PHOTO:
			// need to make sure that photo has actually been taken and saved
			// send "New Photo: [filesize in bytes]"
			// read N bytes from camera
			// send N bytes to server
			// if done, send DONE
			// uip_send(photo_chunk, size);
			break;

			case AUTH_PHOTO:
				if (uip_len > 2 && uip_buf[0] == 'o' && uip_buf[1] == 'k') {
					so.permission = 1;
				}
				break;

			default:
				break;

		}

	}

}
