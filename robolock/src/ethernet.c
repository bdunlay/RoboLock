#include "uip.h"
#include "uip_arp.h"
#include "network-device.h"
#include "hello-world.h"
#include "timer.h"
#include "led.h"
#include "common.h"
#include "LCD.h"

/*---------------------------------------------------------------------------*/

void error(int error) {
	while(1) {
		  printLED(error);
		  busyWait(5);
		  printLED(0);
		  busyWait(5);
	}
}

int
ethernet(void)
{

  uip_ipaddr_t ipaddr;

  if (FALSE == EMACInit()) {
	  error(1);
  }

  uip_init();


  uip_ipaddr(ipaddr, 192,168,0,2);
  uip_sethostaddr(ipaddr);
  hello_world_init();

  while(1) {

	  // read emac, place data in uip buffer
	  uip_len = EMACReceive((DWORD*)uip_buf);

	  // if there is data in the buffer, process it with UIP
	  if (uip_len > 0) {
		  // processes with application handlers
		  uip_input();
	  }
	  // if there's data to send, send it
	  if (uip_len > 0)
		  EMACSend( (DWORD*)uip_buf , UIP_LLH_LEN);
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
