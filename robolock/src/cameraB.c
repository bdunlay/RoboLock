#include "LPC23xx.h"
#include "uart2.h"
#include "cameraB.h"
#include "type.h"
#include "target.h"
#include "common.h"
#include "led.h"
#include "LCD.h"



void testCamera(void){
	a = 0x0000;

	IENABLE;
	endFC = 0;
	//cameraInit();
	cameraReset();
	busyWait(4000);
	cameraTake();
	busyWait(10);
//	cameraTake2();
//	busyWait(10);
	while(!endFC){
	cameraData();
	busyWait(30);
	}
}

void cameraInit(void){
	DWORD Length = 15;
	BYTE BufferPtr[Length];
	BufferPtr[0]=0x36;
	BufferPtr[1]=0x32;
	BufferPtr[2]=0x35;
	BufferPtr[3]=0x0D;
	BufferPtr[4]=0x0A;
	BufferPtr[5]=0x49;
	BufferPtr[6]=0x6E;
	BufferPtr[7]=0x69;
	BufferPtr[8]=0x74;
	BufferPtr[9]=0x20;
	BufferPtr[10]=0x65;
	BufferPtr[11]=0x6E;
	BufferPtr[12]=0x64;
	BufferPtr[13]=0x0D;
	BufferPtr[14]=0x0A;
	UART2Send(BufferPtr,Length );
}

void cameraData(void){
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
	a+=0x20;
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
void cameraTake2(void)
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

int photoSize(void)
{
	DWORD Length = 5;
	BYTE BufferPtr[Length];
	BufferPtr[0]=0x56;
	BufferPtr[1]=0x00;
	BufferPtr[2]=0x34;
	BufferPtr[3]=0x01;
	BufferPtr[4]=0x00;
	UART2Send(BufferPtr, Length);

	//need to find some way to return XH XL of return string

}

void cameraRead(void)
{

}

void cameraStop(void)
{

}


