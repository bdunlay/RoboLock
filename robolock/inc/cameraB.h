#ifndef __CAMERAB_H
#define __CAMERAB_H
void testCamera(void);
void cameraReset(void);
void cameraTake(void);
void cameraTake2(void);
void cameraRead(void);
void cameraStop(void);
void cameraInit(void);
void cameraData(void);

volatile BYTE cameraValue;
volatile int cameraCount;

#endif
