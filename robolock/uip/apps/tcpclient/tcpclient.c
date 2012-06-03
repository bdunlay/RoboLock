#include "tcpclient.h"
#include "uip.h"
#include "led.h"
#include "type.h"
#include "common.h"
#include "timer.h"
#include "robolock.h"
#include "cameraB.h"
#include "uart.h"
#include "LCD.h"
#include "code.h"

void connect(void);
void client(void);
void server(void);
int parse(char* );

void tcp_client_init(void) {
	connect();
    //uip_listen(HTONS(2012));

}

void connect() {

	uip_ipaddr_t ipaddr[2];

	//uip_ipaddr(ipaddr, 184,189,241,29); // home
	//uip_ipaddr(ipaddr, 128,111,43,14);  // CSIL
	uip_ipaddr(ipaddr, 128,111,56,203); // CELAB13 server
	uip_connect(ipaddr, HTONS(9090)); // server port

}

int UIP_LOCK = 0;
void tcp_client_appcall(void) {
	   switch(uip_conn->lport) {
		   case HTONS(2012):
			   server();
			   break;
		   default:
			  client();
			  break;
	   }
}











void client() {


	if (uip_aborted()) {

		printLED(0x1);
		connect();

	}
	if (uip_timedout()) {

		printLED(0x3);
		connect();

	}
	if (uip_closed()) {

		printLED(0x7);
		connect();
	}
	if (uip_connected()) {
		uip_send("hello", 5);
		so.connected = 1;
		printLED(0xF);
	}
	if (uip_acked()) {

		printLED(0x1F);
	}
	if (uip_newdata()) {

		printLED(0x3F);
	}
	//
	//	if (uip_rexmit() ){
	//		ui_send(data_buffer, length);
	//	}

	if (uip_rexmit() || uip_newdata() || uip_acked() || uip_connected() || uip_poll()) {

		if (so.state == SEND_PHOTO && so.send_data_flag) {
			printLED(++UIP_LOCK);
			uip_send(so.dataBuffer, so.chunk_length);
			so.data_sent = 1;
		}




		if (so.state == AUTH_PHOTO && uip_newdata()) {
			if (uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN] == 'O'
					&& uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN + 1] == 'K') {
				so.permission = 1;
			}
		}

	}


	if (uip_newdata()) {

		/* IMG/: TAKE IMAGE ON DEMAND */
		if (uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN] == 'I' && uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN + 1] == 'M' && uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN + 2] == 'G') {
			so.covert = 1;
			update_state(SEND_PHOTO);
		}

		/* DEL/CODE: DELETE CODE */
		if (uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN] == 'D' && uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN + 1] == 'E' && uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN + 2] == 'L') {
			BYTE asciiCodeToInt[4];
			int i;

			for (i = 0; i < 4; i++) {
				asciiCodeToInt[i] = atoi(uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN + 4 + i]);
			}

			if(invalidateOldCode((BYTE*)asciiCodeToInt)) {
				uip_send("DELETE/OK", 9);
			} else {
				uip_send("DELETE/NO", 9);
			}
		}

		/* GET/: GET ALL CODES DELIMITED BY NEWLINES */
		if (uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN] == 'G' && uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN + 1] == 'E' && uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN + 2] == 'T') {
			int codeLength = putCodes((BYTE*)so.prePacketBuffer);
			int length = formatPacket("codes\0", so.prePacketBuffer, codeLength);
			uip_send(so.dataBuffer, length);
		}

		/* SET/CODE: SET NEW CODE */
		if (uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN] == 'S' && uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN + 1] == 'E' && uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN + 2] == 'T') {
			BYTE asciiCodeToInt[4];
			int i;
			for (i = 0; i < 4; i++) {
				asciiCodeToInt[i] = atoi(uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN + 4 + i]);
			}
			addNewCode(asciiCodeToInt, NO_EXPIRE);
		}

		/* TXT/MESSAGE: DISPLAY MESSAGE ON LCD */
		if (uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN] == 'S' && uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN + 1] == 'E' && uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN + 2] == 'T') {

		}


	}

}












enum network_states {
	WAIT,
	NETWORK_PROMPT,
	TOGGLE_NETWORK,
	SET_LOCAL_IP,
	CONFIGURE_NETWORK,
	PARSE_CHOICE
};

struct configuration {
	int network_state;
	uip_ipaddr_t remoteIP[2];
	uip_ipaddr_t localIP[2];
	uip_ipaddr_t gateway[2];
	uip_ipaddr_t netmask[2];
	int state;
} config;


void server() {


	if (uip_aborted()) {

		printLED(0x1);
		connect();

	}
	if (uip_timedout()) {

		printLED(0x3);
		connect();

	}
	if (uip_closed()) {

		printLED(0x7);
		connect();
	}
	if (uip_connected()) {
		config.state = NETWORK_PROMPT;
		printLED(0xF);
	}
	if (uip_acked()) {

		printLED(0x1F);
	}
	if (uip_newdata()) {

		printLED(0x3F);
	}

	if (uip_rexmit() || uip_newdata() || uip_acked() || uip_connected() || uip_poll()) {



		switch (config.state) {

			case WAIT:
				break;

			case NETWORK_PROMPT:
				uip_send("Welcome to RoboLock.\n(1) toggle connectivity\n(2) set local IP address\n(3) set network options.\n", 95);
				config.state = PARSE_CHOICE;
				break;

			case PARSE_CHOICE:
				if (uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN] == '1') {
					config.state = TOGGLE_NETWORK;
				}

				if (uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN] == '2') {
					config.state = SET_LOCAL_IP;
				}

				if (uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN] == '3') {
					config.state = CONFIGURE_NETWORK;
				}
				break;


			case TOGGLE_NETWORK:

				if (config.network_state) {
					config.network_state = 0;
					uip_send("Network DISABLED\n", 17);
				} else {
					config.network_state = 1;
					uip_send("Network ENABLED\n", 16);
				}

				config.state = WAIT;

				break;

			case SET_LOCAL_IP:
				break;

			case CONFIGURE_NETWORK:
				break;

			default:
				break;

		}


	}


}
