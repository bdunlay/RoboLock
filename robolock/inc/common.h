#ifndef __COMMON_H 
#define __COMMON_H



void busyWait(DWORD);
BYTE hexToChar(BYTE);

volatile BYTE keypadValue;
volatile int keypadCount;

volatile BYTE cameraValue;
volatile int cameraCount;

volatile DWORD endFC;
volatile BYTE picSize;

#endif
