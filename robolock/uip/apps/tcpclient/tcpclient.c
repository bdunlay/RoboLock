#include "tcpclient.h"
#include "uip.h"
#include "led.h"
#include "type.h"
#include "common.h"
#include "timer.h"
#include "robolock.h"
#include "cameraB.h"
#include "uart.h"

void connect(void);

void tcp_client_init(void) {
	connect();
}

void connect() {
	uip_ipaddr_t ipaddr[2];

	//uip_ipaddr(ipaddr, 184,189,241,29); // server address
	uip_ipaddr(ipaddr, 128,111,56,203); // server address
	uip_connect(ipaddr, HTONS(9090)); // server port

}

int UIP_LOCK = 0;
void tcp_client_appcall(void) {

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

}


//	if (uip_aborted() || uip_timedout() || uip_closed()) {
//		printLED(dancything);
//		dancything = ~dancything;
//		so.connected = 0;
//
//		connect();
//	}
//
//	if (uip_connected()) {
//		so.connected = 1;
//		uip_send("Hello, server!", 14);
//		UIPSTATE = 1;
//	}
//
//	if (so.state == AUTH_PHOTO || so.state == SEND_PHOTO) {
//		printLED(0x33);
////		printLED(0x1 << dancything2++ % 8);
//
//		int length;
//
//		switch (so.state) {
//
//		case SEND_PHOTO:
//		//UARTprint(" [Send Photo UIP State] \0");
//			if (so.photo_address < so.photo_size) {
//
//				int count = JPEGCamera_readData(so.jpegResponse,
//						so.photo_address);
//				int i;
//
//				for (i = 5; i < count - 5; i++) {
//					//Check the response for the eof indicator (0xFF, 0xD9). If we find it, set the eof flag
//					if ((so.jpegResponse[i] == (char) 0xD9) && (so.jpegResponse[i - 1] == (char) 0xFF))
//						eof = 1;
//
//					chunkBuffer[i-5] = so.jpegResponse[i];
//					UARTSendChar((BYTE)so.jpegResponse[i]);
//
//					if (eof == 1)
//						break;
//				}
//
//				so.photo_address += (count - 10);
//
//			} else {
//				length = formatPacket("photo\0", "END", 3);
//				so.photo_sent = 1;
//				update_state(AUTH_PHOTO);
//			}
//
//			uip_send(dataBuffer, length);
//			break;
//
//		case AUTH_PHOTO:
//
//			if (uip_newdata()) {
//				if (uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN] == 'O'
//						&& uip_buf[UIP_LLH_LEN + UIP_TCPIP_HLEN + 1] == 'K') {
//					so.permission = 1;
//				}
//			}
//			break;
//
//		default:
//			length = formatPacket("heartbeat\0", "thump thump", 11);
//			uip_send(dataBuffer, length);
//			// heartbeat to indicate that we're connected
//			//			for (k = 0; k < 8; k++) {
//			//				printLED(1 << k);
//			//				busyWait(20);
//			//			}
//
//			break;
//		}
//	}

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


