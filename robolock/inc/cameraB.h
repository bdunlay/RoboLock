// cameraB.h

#ifndef __CAMERAB_H
#define __CAMERAB_H

BYTE MH, ML;
int a;

void testCamera(void);
void cameraReset(void);
void cameraTake(void);
void getPhotoSize(void);
void cameraRead(void);
void cameraStop(void);
void cameraInit(void);
void cameraData(int);
BYTE cameraSize(void);
int getChunk(BYTE**,int);








void JPEGCamera_begin(void);
void JPEGCamera_test(void);
int JPEGCamera_reset(char*);
int JPEGCamera_sendCommand(const char*, char*, int);
int JPEGCamera_takePicture(char *);
int JPEGCamera_stopPictures(char *);
int JPEGCamera_getSize(char *, int *);
int JPEGCamera_readData(char * , int );

volatile BYTE cameraValue;
volatile int cameraCount;

volatile DWORD endFC;
volatile DWORD camSize;
volatile BYTE picSize;

#endif
