#ifndef __TCPCLIENT_R_H
#define __TCPCLIENT_R_H

#include "uipopt.h"

typedef struct tcp_client_appstate {
  char inputbuffer[10];
  char name[40];
} uip_tcp_appstate_t;


void tcp_client_init( void );
void tcp_client_appcall( void );



#endif
