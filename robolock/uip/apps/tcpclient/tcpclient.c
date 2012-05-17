#include "tcpclient.h"
#include "uip.h"
#include "led.h"
#include "type.h"
#include "common.h"

void tcp_client_init(void) {

	uip_listen(HTONS(8080));



}

void tcp_client_appcall(void) {
   if(uip_newdata() || uip_rexmit()) {
      uip_send("HELLO WORLD!!!!!\n", 17);
   }
}
