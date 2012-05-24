#include "tcpclient.h"
#include "uip.h"
#include "led.h"
#include "type.h"
#include "common.h"
#include "timer.h"
#include "robolock.h"

void connect(void);

void tcp_client_init(void) {

	connect();

}

int state = 0;

void connect() {
	u16_t ipaddr[2];

	uip_ipaddr(ipaddr, 128,111,56,203); // server address
	uip_connect(ipaddr, HTONS(9090)); // serer port

}

int startByte, endByte, totalBytes;

char dataBuffer[1024];

int formatPacket(char* type, char* data, int bytes) {

	int i, k;

	for (i = 0; i < 1024; i++) {
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

int mm = 0;
int counter;

void tcp_client_appcall(void) {
	int k;

	if (uip_aborted() || uip_timedout() || uip_closed()) {
		printLED(mm);
		busyWait(1000);
		mm = ~mm;

		//		update_state(ERROR);
		//		uip_close(); // do i want to do this?

		connect();
	} else if (uip_connected() || uip_newdata()) {
		counter++;

		int length;

		switch (so.state) {

		case PHOTO:
			if (1/*photoChunkSize() && uip_acked()*/) { // TODO photoChunkSize should return next chunk size
				length // TODO photoChunk should return next photo chunk in buffer
						= formatPacket("photo\0", "1"/*photoChunk()*/,
								1/*photoChunkSize()*/);
			} else {
				length = formatPacket("photo\0", "END", 3);
				so.photo_sent = 0;
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
			if (counter >= 1000) {
				length = formatPacket("heartbeat\0", "thump thump", 11);
				uip_send(dataBuffer, length);
				// heartbeat to indicate that we're connected
				for (k = 0; k < 8; k++) {
					printLED(1 << k);
					busyWait(20);
				}
			}

			counter = 0;
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
