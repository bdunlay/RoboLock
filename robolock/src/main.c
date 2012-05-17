#define __MAIN_C__

#include "type.h"
#include "LPC23xx.h"
#include "target.h"
#include "common.h"
#include "led.h"
#include "dip.h"
#include "LCD.h"
#include "ex_sdram.h"
#include "timer.h"
#include "uart.h"
#include "adc.h"
#include "uart.h"
#include "ethernet.h"
#include "strike.h"
#include "robolock.h"
#define UIP_ENABLED 0

#if UIP_ENABLED

//#include "uip_timer.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "clock-arch.h"
#include "clock.h"
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#endif



/*****************************************************************************
 *    Main Function  main()													 *
 *****************************************************************************/

int main (void)
{

	TargetResetInit();
	initLED();
	initLCD();
	init_dip();

	UARTInit(9600);
	ADCInit();
	SDRAMInit();
	strikeInit();


#if UIP_ENABLED


	unsigned int iter;
	uip_ipaddr_t ipaddr;	/* local IP address */
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
	uip_ipaddr(ipaddr, 169,254,255,255);
	uip_sethostaddr(ipaddr);	/* host IP address */

//	uip_ipaddr(ipaddr, 128,111,56,201);
//	uip_sethostaddr(ipaddr);	/* host IP address */
//	uip_ipaddr(ipaddr, 128,168,51,1);
//	uip_setdraddr(ipaddr);	/* router IP address */
//	uip_ipaddr(ipaddr, 255,255,255,0);
//	uip_setnetmask(ipaddr);	/* mask */

	// Initialize the HTTP server, listen to port 80.
	tcp_client_init();
//	printLED(1);
//	uip_input();
//	printLED(2);
//	uip_input();
//	printLED(3);
//	tapdev_send(uip_buf,uip_len);
	printLED(4);

//	while(1) {
//		uip_input();
//		printLED(uip_len);
//		busyWait(500);
//		printLED(255);
//		busyWait(100);
//
//		tapdev_send(uip_buf,uip_len);

//	}

#endif




	while(1) {


		switch(read_dip()) {



			/* MAIN PROGRAM */

			case 0:
				robolock();
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



				printLED(1);



				uip_len = tapdev_read(uip_buf);
				if(uip_len > 0)		/* received packet */
				{
					printLED(2);
			  		if(BUF->type == htons(UIP_ETHTYPE_IP))	/* IP packet */
			  		{
			      		uip_arp_ipin();
			      		uip_input();
			      		/* If the above function invocation resulted in data that
			         		should be sent out on the network, the global variable
			         		uip_len is set to a value > 0. */

			      		if(uip_len > 0)
			    		{
			        		uip_arp_out();
			        		tapdev_send(uip_buf,uip_len);
			      		}
			  		}
			      	else if(BUF->type == htons(UIP_ETHTYPE_ARP))	/*ARP packet */
			      	{
			        	uip_arp_arpin();
				      	/* If the above function invocation resulted in data that
				         	should be sent out on the network, the global variable
				         	uip_len is set to a value > 0. */
				      	if(uip_len > 0)
			        	{
				      		printLED(255);
				      		busyWait(100);
				      		printLED(0);
				      		busyWait(100);
				        	tapdev_send(uip_buf,uip_len);	/* ARP ack*/
				      	}
			      	}
				}
//				else if(timer_expired(&periodic_timer))	/* no packet but periodic_timer time out (0.5s)*/
//				{
//			  		timer_reset(&periodic_timer);
//
//			  		for(iter = 0; iter < UIP_CONNS; iter++)
//			  		{
//			  			uip_periodic(iter);
//				        /* If the above function invocation resulted in data that
//				           should be sent out on the network, the global variable
//				           uip_len is set to a value > 0. */
//				        if(uip_len > 0)
//				        {
//				          uip_arp_out();
//				          tapdev_send(uip_buf,uip_len);
//				        }
//			  		}
//			#if UIP_UDP
//					for(iter = 0; iter < UIP_UDP_CONNS; iter++) {
//						uip_udp_periodic(iter);
//						/* If the above function invocation resulted in data that
//						   should be sent out on the network, the global variable
//						   uip_len is set to a value > 0. */
//						if(uip_len > 0) {
//						  uip_arp_out();
//						  tapdev_send();
//						}
//					}
//			#endif /* UIP_UDP */
//			     	/* Call the ARP timer function every 10 seconds. */
//					if(timer_expired(&arp_timer))
//					{
//						timer_reset(&arp_timer);
//						uip_arp_timer();
//					}
//				}





			#endif

				break;

			case 8:
				testSDRAM();
				break;

			case 9:
				testSDRAM_simple();
				break;

			case 10:
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


