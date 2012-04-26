/* RoboLock
 * dip.h
 *
 * Source file for GPIO control, configuration and usage of
 * the dip switch on the RoboLock board
 *
 * v1.0 - 4/22/2012
 */

#ifndef __DIP_H
#define __DIP_H

void init_dip(void);
unsigned short read_dip(void);

#endif
