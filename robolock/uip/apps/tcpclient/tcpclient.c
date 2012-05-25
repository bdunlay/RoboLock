#include "tcpclient.h"
#include "uip.h"
#include "led.h"
#include "type.h"
#include "common.h"
#include "timer.h"
#include "robolock.h"
#include "cameraB.h"

void connect(void);

void tcp_client_init(void) {

	connect();

}

int state = 0;

void connect() {

	u16_t ipaddr[2];

	//uip_ipaddr(ipaddr, 184,189,241,29); // server address
	uip_ipaddr(ipaddr, 128,111,56,203); // server address

	uip_connect(ipaddr, HTONS(9090)); // server port

}

int startByte, endByte, totalBytes;

char dataBuffer[64];

int formatPacket(char* type, char* data, int bytes) {

	int i, k;

	for (i = 0; i < 64; i++) {
		dataBuffer[i] = 0;
	}

	for (i = 0; type[i] != '\0'; i++) {
		dataBuffer[i] = type[i];
	}

	dataBuffer[i++] = '/';

	for (k = 0; k < bytes; k++) {
		dataBuffer[i++] = data[k];
	}

	return i;
}

int mm = 0xAA;
int counter;

BYTE chunkBuffer[64];
int chunkSize;

void tcp_client_appcall(void) {
	int k;
	chunkSize = 0;

	if (uip_aborted() || uip_timedout() || uip_closed()) {
//		printLED(mm);
//		mm = ~mm;
		//busyWait(1000);

		//		update_state(ERROR);
		//		uip_close(); // do i want to do this?

		connect();
	} else if (uip_connected() || uip_newdata()) {
		counter++;

		int length;

		switch (so.state) {

		case SEND_PHOTO:

			chunkSize = getChunk(&chunkBuffer, 0x20);

			if (chunkSize) {
				length = formatPacket("photo\0", chunkBuffer, chunkSize);
			} else {
				length = formatPacket("photo\0", "END", 3);
				so.photo_sent = 1;
			}

			uip_send(dataBuffer, length);

			break;
		case AUTH_PHOTO:
			if (uip_newdata()) {
				if (uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN] == 'O'
						&& uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN + 1] == 'K') {
					so.permission = 1;
				}
			}
			break;

		default:
			length = formatPacket("heartbeat\0", "thump thump", 11);
			uip_send(dataBuffer, length);
			// heartbeat to indicate that we're connected
//			for (k = 0; k < 8; k++) {
//				printLED(1 << k);
//				busyWait(20);
//			}
			counter = 0;

			break;

		}

		//		if (START) {
		//			START = 0;
		//			uip_send(dataBuffer, length);
		//
		//		} else if (END)
		//
		//		if (uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN]== 'O' && uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN + 1] == 'K') {
		//			END = 0;
		//
		//		}


		//		switch (so.state) {
		//
		//		case PHOTO:
		//			// need to make sure that photo has actually been taken and saved
		//			// send "New Photo: [filesize in bytes]"
		//			// read N bytes from camera
		//			// send N bytes to server
		//			// if done, send DONE
		//			// uip_send(photo_chunk, size);
		//
		//			totalBytes = getPhotoSize();
		//
		//			formatPacket("photo", startByte, endByte, totalbytes, getChunk());
		//
		//
		//			break;
		//
		//		case AUTH_PHOTO:
		//			if (uip_len > 2 && uip_buf[0] == 'o' && uip_buf[1] == 'k') {
		//				so.permission = 1;
		//			}
		//			break;
		//
		//		default:
		//			break;
		//
		//		}

	}

}
