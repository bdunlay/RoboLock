#include "LPC23xx.h"
#include "uart2.h"
#include "cameraB.h"
#include "type.h"
#include "target.h"
#include "common.h"
#include "led.h"
#include "LCD.h"
#include "uart.h"
#include "robolock.h"



void testCamera(void){

//	BYTE chunkBuffer[0x20];

	cameraTake();
	getPhotoSize();
	//	getChunk(&chunkBuffer, 0x20);



	while(1);

}

void cameraInit(void){

	a = 0x0000;
	cameraCount = 0;
	camSize = 0;
	IENABLE;
	endFC = 0;

//	DWORD Length = 15;
//	BYTE BufferPtr[Length];
//	BufferPtr[0]=0x36;
//	BufferPtr[1]=0x32;
//	BufferPtr[2]=0x35;
//	BufferPtr[3]=0x0D;
//	BufferPtr[4]=0x0A;
//	BufferPtr[5]=0x49;
//	BufferPtr[6]=0x6E;
//	BufferPtr[7]=0x69;
//	BufferPtr[8]=0x74;
//	BufferPtr[9]=0x20;
//	BufferPtr[10]=0x65;
//	BufferPtr[11]=0x6E;
//	BufferPtr[12]=0x64;
//	BufferPtr[13]=0x0D;
//	BufferPtr[14]=0x0A;
//	UART2Send(BufferPtr,Length );

	cameraReset();

}

// requests data of si
void cameraData(int size){
	DWORD Length = 16;
	MH = a/0x100;
	ML = a%0x100;
	BYTE BufferPtr[Length];
	BufferPtr[0]=0x56;
	BufferPtr[1]=0x00;
	BufferPtr[2]=0x32;
	BufferPtr[3]=0x0C;
	BufferPtr[4]=0x00;
	BufferPtr[5]=0x0A;
	BufferPtr[6]=0x00;
	BufferPtr[7]=0x00;
	BufferPtr[8]=MH;
	BufferPtr[9]=ML;
	BufferPtr[10]=0x00;
	BufferPtr[11]=0x00;
	BufferPtr[12]=0x00;
	BufferPtr[13]=0x20;
	BufferPtr[14]=0x00;
	BufferPtr[15]=0x0A;
	UART2Send(BufferPtr,Length );
	a+=size;
}

void cameraReset(void){
	DWORD Length = 4;
	BYTE BufferPtr[Length];
	BufferPtr[0]=0x56;
	BufferPtr[1]=0x00;
	BufferPtr[2]=0x26;
	BufferPtr[3]=0x00;
	UART2Send(BufferPtr,Length );

}


/* sends command to take picture */
void cameraTake(void)
{
	DWORD Length = 5;
	BYTE BufferPtr[Length];
	BufferPtr[0]=0x56;
	BufferPtr[1]=0x00;
	BufferPtr[2]=0x36;
	BufferPtr[3]=0x01;
	BufferPtr[4]=0x00;
	UART2Send(BufferPtr, Length );

}
void getPhotoSize(void)
{
	DWORD Length = 5;
	BYTE BufferPtr[Length];
	BufferPtr[0]=0x56;
	BufferPtr[1]=0x00;
	BufferPtr[2]=0x34;
	BufferPtr[3]=0x01;
	BufferPtr[4]=0x00;
	UART2Send(BufferPtr, Length );

}

BYTE cameraSize(void)
{
	DWORD Length = 5;
	BYTE BufferPtr[Length];
	BufferPtr[0]=0x56;
	BufferPtr[1]=0x00;
	BufferPtr[2]=0x34;
	BufferPtr[3]=0x01;
	BufferPtr[4]=0x00;
	UART2Send(BufferPtr, Length);

	if(camSize)
		return cameraValue;
	//need to find some way to return XH XL of return string
	else
		return -1;
}

void cameraRead(void)
{

}

void cameraStop(void)
{

}


int getChunk(BYTE** buffer, int size){

	cameraData(size);

	while(!endFC);


	//DWORD length = 32;
	//BYTE printBuf[length];
	printLED(0xF0);
	int i;
	for(i = 0; i<size ||(UART2Buffer[i] == 0xD9); i++) {
		*buffer[i] = UART2Buffer[i];
		printLED(i);
		busyWait(200);
		//printBuf[i] = *buffer[i];
	}
	printLED(0x10);
	//UARTSend(printBuf, length);
	//printLED(*buffer[31]);
	busyWait(10);

	lcdBacklight();
	init_robolock();

	lcdInit();
	lcdDisplay(*buffer, &((*buffer)[31]));

	endFC = 0;
	return i;

}


