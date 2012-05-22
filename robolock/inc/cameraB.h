#ifndef __CAMERAB_H
#define __CAMERAB_H

BYTE MH, ML;
int a;

void testCamera(void);
void cameraReset(void);
void cameraTake(void);
void cameraTake2(void);
void cameraRead(void);
void cameraStop(void);
void cameraInit(void);
void cameraData(void);
int photoSize(void);

volatile BYTE cameraValue;
volatile int cameraCount;

volatile DWORD endFC;
volatile BYTE picSize;

#endif
