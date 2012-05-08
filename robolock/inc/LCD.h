/** RoboLock
 * LED.H
 *
 * Header file for i2c control for LCD
 *
 * v1.0 - 4/22/2012
 */

#ifndef __LCD_H
#define __LCD_H

#include "I2C.h"

void initLCD(void);
void clearLCD(void);
void printLCD(BYTE);
void lcdStart(void);
void testLCD(void);
void backlightLCD(void);

#endif
