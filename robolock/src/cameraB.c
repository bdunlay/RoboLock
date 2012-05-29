// cameraB.c

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


//We read data from the camera in chunks, this is the chunk size
#define READ_SIZE 32 //TODO increase camera read size


const char GET_SIZE[5] = {0x56, 0x00, 0x34, 0x01, 0x00};
const char RESET_CAMERA[4] = {0x56, 0x00, 0x26, 0x00};
const char TAKE_PICTURE[5] = {0x56, 0x00, 0x36, 0x01, 0x00};
const char STOP_TAKING_PICS[5] = {0x56, 0x00, 0x36, 0x01, 0x03};
char READ_DATA[16] = {0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A};

void JPEGCamera_begin(void)
{
	//Camera baud rate is 38400
	UART2Init(38400);
}

int JPEGCamera_reset(char* response)
{
	return JPEGCamera_sendCommand(RESET_CAMERA, response, 4);
}

int JPEGCamera_takePicture(char * response)
{
	return JPEGCamera_sendCommand(TAKE_PICTURE, response, 5);
}

int JPEGCamera_stopPictures(char * response)
{
	return JPEGCamera_sendCommand(STOP_TAKING_PICS, response, 5);
}

int JPEGCamera_getSize(char * response, int * size)
{
	int count=0;
	//Send the GET_SIZE command string to the camera
	count = JPEGCamera_sendCommand(GET_SIZE, response, 5);

	//The size is in the last 2 characters of the response.
	//Parse them and convert the characters to an integer
    *size = response[count-2]*256;
    *size += (int)response[count-1] & 0x00FF;
	//Send the number of characters in the response back to the calling function
	return count;
}

int JPEGCamera_readData(char * response, int address)
{
	int count=0;

	//Send the command to get read_size bytes of data from the current address

	READ_DATA[8] = address>>8;
	READ_DATA[9] = address;

	READ_DATA[12] = READ_SIZE>>8;
	READ_DATA[13] = READ_SIZE;

	count = JPEGCamera_sendCommand(READ_DATA, response, 16);

	return count;
}



int JPEGCamera_sendCommand(const char* command, char* response, int length)
{
	int count=0;

	//Send each character in the command string to the camera through the camera serial port
	UART2Send(command,length);
	busyWait(50); // TODO increase with larger buffer sizes
	//Get the response from the camera and add it to the response string.
	count = UART2Read(response);

	//return the number of characters in the response string
	return count;

}












void JPEGCamera_test(void) {
//	char JPEG_response[64] = {};
//	//Size will be set to the size of the jpeg image.
//	int size=0;
//	//This will keep track of the data address being read from the camera
//	//eof is a flag for the sketch to determine when the end of a file is detected
//	//while reading the file data from the camera.
//	int eof=0;
//
//	JPEGCamera_begin();
//
//	int i = 0;
//	int x = JPEGCamera_reset(JPEG_response);
//	busyWait(3000);
//
//	x = JPEGCamera_takePicture(JPEG_response);
//	x = JPEGCamera_getSize(JPEG_response, &size);
//
//	int count;
//	int address = 0;
//	int printCounter = 0;
//	while(address < size) {
//    	printLED(++printCounter);
//        //Read the data starting at the current address.
//        count=JPEGCamera_readData(JPEG_response, address);
//        //Store all of the data that we read to the SD card
//
//        for(i=5; i<count-5; i++){
//            //Check the response for the eof indicator (0xFF, 0xD9). If we find it, set the eof flag
//            if((JPEG_response[i] == (char)0xD9) && (JPEG_response[i-1]==(char)0xFF))eof=1;
//            UARTSendChar(JPEG_response[i]);
//            if(eof==1)break;
//        }
//
//        //Increment the current address by the number of bytes we read
//        address+=(count-10);
//        //Make sure we stop reading data if the eof flag is set.
//        if(eof==1)break;
//    	x = JPEGCamera_stopPictures(JPEG_response);
//
//    }

}





















void testCamera(void){
//
////	BYTE chunkBuffer[0x20];
//
//	cameraTake();
//	getPhotoSize();
//	//	getChunk(&chunkBuffer, 0x20);



	while(1);

}

void cameraInit(void){

//	a = 0x0000;
//	cameraCount = 0;
//	camSize = 0;
//	IENABLE;
//	endFC = 0;
//
////	DWORD Length = 15;
////	BYTE BufferPtr[Length];
////	BufferPtr[0]=0x36;
////	BufferPtr[1]=0x32;
////	BufferPtr[2]=0x35;
////	BufferPtr[3]=0x0D;
////	BufferPtr[4]=0x0A;
////	BufferPtr[5]=0x49;
////	BufferPtr[6]=0x6E;
////	BufferPtr[7]=0x69;
////	BufferPtr[8]=0x74;
////	BufferPtr[9]=0x20;
////	BufferPtr[10]=0x65;
////	BufferPtr[11]=0x6E;
////	BufferPtr[12]=0x64;
////	BufferPtr[13]=0x0D;
////	BufferPtr[14]=0x0A;
////	UART2Send(BufferPtr,Length );
//
//	cameraReset();

}

// requests data of si
void cameraData(int size){
//	DWORD Length = 16;
//	MH = a/0x100;
//	ML = a%0x100;
//	BYTE BufferPtr[Length];
//	BufferPtr[0]=0x56;
//	BufferPtr[1]=0x00;
//	BufferPtr[2]=0x32;
//	BufferPtr[3]=0x0C;
//	BufferPtr[4]=0x00;
//	BufferPtr[5]=0x0A;
//	BufferPtr[6]=0x00;
//	BufferPtr[7]=0x00;
//	BufferPtr[8]=MH;
//	BufferPtr[9]=ML;
//	BufferPtr[10]=0x00;
//	BufferPtr[11]=0x00;
//	BufferPtr[12]=0x00;
//	BufferPtr[13]=0x20;
//	BufferPtr[14]=0x00;
//	BufferPtr[15]=0x0A;
//	UART2Send(BufferPtr,Length );
//	a+=size;
}

void cameraReset(void){
//	DWORD Length = 4;
//	BYTE BufferPtr[Length];
//	BufferPtr[0]=0x56;
//	BufferPtr[1]=0x00;
//	BufferPtr[2]=0x26;
//	BufferPtr[3]=0x00;
//	UART2Send(BufferPtr,Length );

}


/* sends command to take picture */
void cameraTake(void)
{
//	DWORD Length = 5;
//	BYTE BufferPtr[Length];
//	BufferPtr[0]=0x56;
//	BufferPtr[1]=0x00;
//	BufferPtr[2]=0x36;
//	BufferPtr[3]=0x01;
//	BufferPtr[4]=0x00;
//	UART2Send(BufferPtr, Length );

}
void getPhotoSize(void)
{
//	DWORD Length = 5;
//	BYTE BufferPtr[Length];
//	BufferPtr[0]=0x56;
//	BufferPtr[1]=0x00;
//	BufferPtr[2]=0x34;
//	BufferPtr[3]=0x01;
//	BufferPtr[4]=0x00;
//	UART2Send(BufferPtr, Length );

}

BYTE cameraSize(void)
{
//	DWORD Length = 5;
//	BYTE BufferPtr[Length];
//	BufferPtr[0]=0x56;
//	BufferPtr[1]=0x00;
//	BufferPtr[2]=0x34;
//	BufferPtr[3]=0x01;
//	BufferPtr[4]=0x00;
//	UART2Send(BufferPtr, Length);
//
//	if(camSize)
//		return cameraValue;
//	//need to find some way to return XH XL of return string
//	else
//		return -1;
	return 0;
}

void cameraRead(void)
{

}

void cameraStop(void)
{

}


int getChunk(BYTE** buffer, int size){

//	cameraData(size);
//
//	while(!endFC);
//
//
//	//DWORD length = 32;
//	//BYTE printBuf[length];
//	printLED(0xF0);
//	int i;
//	for(i = 0; i<size ||(UART2Buffer[i] == 0xD9); i++) {
//		*buffer[i] = UART2Buffer[i];
//		printLED(i);
//		busyWait(200);
//		//printBuf[i] = *buffer[i];
//	}
//	printLED(0x10);
//	//UARTSend(printBuf, length);
//	//printLED(*buffer[31]);
//	busyWait(10);
//
//	lcdBacklight();
//	init_robolock();
//
//	lcdInit();
//	lcdDisplay(*buffer, &((*buffer)[31]));
//
//	endFC = 0;
//	return i;
return 0;
}


