/* RoboLock
 * LED.H
 *
 * Header file for GPIO control, configuration and usage of LEDs on the RoboLock board
 *
 * v1.0 - 4/22/2012
 */

#ifndef __LED_H
#define __LED_H

void initLED(void);
void clearLED(void);
void printLED(unsigned int);
void testLED(void);

#endif
