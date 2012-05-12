#ifndef TAPDEV_H
#define TAPDEV_H

/**
 * \file
 * interface between uIP and device driver
 */

/// dummy, MAC has to be initialized by explicit call of emac_init
void tapdev_init(void);
unsigned int tapdev_read(void *pPacket);
void tapdev_send(void *pPacket, unsigned int size);

#endif
