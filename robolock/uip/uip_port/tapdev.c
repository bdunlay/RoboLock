#include "tapdev.h"
#include "network-device.h"

void tapdev_init(void)
{
  /// we call our init func elsewhere
	EMACInit();
}

unsigned int tapdev_read(void *pPacket)
{
  EMACReceive(pPacket);//, MAX_PACKET_SIZE);// ETH_FRAG_SIZE);
  return 0;
}

void tapdev_send(void *pPacket, unsigned int size)
{
  EMACSend(pPacket, size);
}

extern DWORD EMACSend( DWORD *EMACBuf, DWORD length );
extern DWORD EMACReceive( DWORD *EMACBuf );
