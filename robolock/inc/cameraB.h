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


//We read data from the camera in chunks, this is the chunk size
#define READ_SIZE 1024 //320 //TODO increase camera read size




void JPEGCamera_begin(void);
void JPEGCamera_test(void);
int JPEGCamera_reset(char*);
int JPEGCamera_sendCommand(const unsigned char*, char*, int);
int JPEGCamera_takePicture(char *);
int JPEGCamera_stopPictures(char *);
int JPEGCamera_getSize(char *, int *);
int JPEGCamera_readData(char * , int );
int JPEGCamera_setRes640x480(char*);
int JPEGCamera_setRes320x240(char*);
int JPEGCamera_setRes160x120(char*);

volatile BYTE cameraValue;
volatile int cameraCount;

volatile DWORD endFC;
volatile DWORD camSize;
volatile BYTE picSize;

#endif
