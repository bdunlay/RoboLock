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

void robolock() {

	// muahahahahaha

	//wait for knock
	lcdInit();
	lcdDisplay("waiting         ","for knock       ");
	busyWait(500);
//	while(1){
//		if(peizoListen()==1);
//			break;
//	}

	//Activate  LCD Message
	lcdBacklight();
			lcdDisplay("Hello           ","                ");
			busyWait(500);
			lcdDisplay("Please          ","Face Camera     ");
			busyWait(500);

	//Take Picture
//			cameraTake();
			busyWait(500);

    //Send Picture to server
//			ethernetSend();

	//wait for response
//			while(1){
//				if(networkFlag)
//					break;
//			}

	//Handle response
//			if(networkResponse== VALID_USER){
			lcdClear();
				lcdDisplay("Acess           ","Granted         ");
				strikeOpen();
				busyWait(500);
				strikeClose();
				lcdBacklightOff();
//			}

}
