#include "cmsis_os.h"  // CMSIS RTOS header file
#include "Board_LED.h"
#include "UART_driver.h"
#include "stdint.h"                     // data type definitions
#include "stdio.h"                      // file I/O functions
#include "rl_usb.h"                     // Keil.MDK-Pro::USB:CORE
#include "rl_fs.h"                      // Keil.MDK-Pro::File System:CORE
#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_audio.h"
#include "audio.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "arm_math.h" // header for DSP library
#include "stdlib.h"

#define NUM_CHAN	2 // number of audio channels
#define NUM_POINTS 1024 // number of points per channel
#define BUF_LEN NUM_CHAN*NUM_POINTS // length of the audio buffer
/* Private variables ---------------------------------------------------------*/
/* buffer used for audio play */
int16_t Audio_Buffer1[BUF_LEN]; // Buffer1
int16_t Audio_Buffer2[BUF_LEN]; // Buffer2

osSemaphoreDef (Sem); // declare semaphore
osSemaphoreId (Sem_id); // semaphore ID

#define MSGQUEUE_OBJECTS 1 // number of Message Queue Objects
osMessageQId mid_MsgQueue;                               // message queue id
osMessageQDef (MsgQueue, MSGQUEUE_OBJECTS, int32_t);     // message queue object

// WAVE file header format
typedef struct WAVHEADER {
	unsigned char riff[4];						// RIFF string
	uint32_t overall_size;				// overall size of file in bytes
	unsigned char wave[4];						// WAVE string
	unsigned char fmt_chunk_marker[4];		// fmt string with trailing null char
	uint32_t length_of_fmt;					// length of the format data
	uint16_t format_type;					// format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	uint16_t channels;						// no.of channels
	uint32_t sample_rate;					// sampling rate (blocks per second)
	uint32_t byterate;						// SampleRate * NumChannels * BitsPerSample/8
	uint16_t block_align;					// NumChannels * BitsPerSample/8
	uint16_t bits_per_sample;				// bits per sample, 8- 8bits, 16- 16 bits etc
	unsigned char data_chunk_header [4];		// DATA string or FLLR string
	uint32_t data_size;						// NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
} WAVHEADER;

void Thread (void const *argument);                             // thread function
osThreadId tid_Thread;                                          // thread id
osThreadDef (Thread, osPriorityNormal, 1, 0); 
void Indexing (void);                             // thread function
void Initializing (void);                             		// thread function
void Playing (void);                             // thread function

WAVHEADER header;
size_t rd;
uint32_t i;
static uint8_t rtrn = 0;
uint8_t rdnum = 1; // read buffer number
/* USART Driver */
extern ARM_DRIVER_USART Driver_USART4;
float32_t Fs = 44100.0; // sample frequency
float32_t tmp; // factor
float32_t cnt = 0.0f; // time index
	
void Init_Thread (void) {
	// initialize the audio output
	rtrn = BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, 0x46, Fs);
	if (rtrn != AUDIO_OK)return;
	
	LED_Initialize();
	UART_Init();
	
	tid_Thread = osThreadCreate (osThread(Thread), NULL);
  if (!tid_Thread) return;
	
	Sem_id = osSemaphoreCreate(osSemaphore(Sem), 0); // initialize semaphore to 0
	mid_MsgQueue = osMessageCreate (osMessageQ(MsgQueue), NULL);  // create msg queue
  if (!mid_MsgQueue) {
    ; // Message Queue object not created, handle failure
  }
}

void Thread (void const *argument) {
	char r_data[2]={0,0}; // Value P = Playing, I = Indexing, N = Initializing
	LED_Off(1);
	LED_Off(2);
	LED_Off(3);
	LED_On(4);
	
	Initializing();
  /*while (1) {
		UART_receive(r_data, 1);
		
		if(!strcmp(r_data,"N")){
			Initializing();
		}
	  if(!strcmp(r_data,"P")){
			Playing();
		}
		else if(!strcmp(r_data,"I")){
			Indexing();
		}
	} // end while*/
} // end Thread

void Initializing (void) {  // Switch to Initializing when GUI starts up
	LED_Off(1);
	LED_Off(2);
	LED_Off(4);
	LED_On(3);
	
	usbStatus ustatus; // USB driver status variable
	uint8_t drivenum = 0; // Using U0: drive number
	char *drive_name = "U0:"; // USB drive name
	fsStatus fstatus; // file system status variable
	static FILE *f;  // pointer to file type for files on USB device
	
	ustatus = USBH_Initialize (drivenum); // initialize the USB Host
	
	if (ustatus == usbOK){
		// loop until the device is OK, may be delay from Initialize
		ustatus = USBH_Device_GetStatus (drivenum); // get the status of the USB device
		while(ustatus != usbOK){
			ustatus = USBH_Device_GetStatus (drivenum); // get the status of the USB device
		}
		// initialize the drive
		fstatus = finit (drive_name);
		if (fstatus != fsOK){
			// handle the error, finit didn't work
		} // end if
		// Mount the drive
		fstatus = fmount (drive_name);
		if (fstatus != fsOK){
			// handle the error, fmount didn't work
		} // end if
		// file system and drive are good to go
		// read directory
		fsFileInfo info;
		info.fileID = 0;
		//UART_send("Init\n",5);
		//char buffer[50];
		
		osDelay(1000);
		
		while (ffind ("*", &info) == fsOK) {     // find whatever is in any drive"
			//sprintf(buffer,"\n%s",info.name);
			//UART_send(buffer,50); // send to UART
			//memset(buffer, 0, 50 * (sizeof buffer[0]) ); // clear buffer
			//UART_send(info.name,strlen(info.name));
			//UART_send("\n",1);
			UART_send(info.name,strlen(info.name)); // send to UART
			UART_send("\n",1);
		}
		//UART_send("Done_Init\n",10);
	} // end if USBH_Initialize	
	
	return;
}

void Indexing (void) {  // Switch to Playing when play button pressed
	LED_Off(1);
	LED_Off(3);
	LED_Off(4);
	LED_On(2);
	
	
	
	return;
}

void Playing (void) {  // Switch to Indexing pause button pressed or song ends
	LED_Off(2);
	LED_Off(3);
	LED_Off(4);
	LED_On(1);

	/*
	f = fopen ("Test.wav","r");// open a file on the USB device
	if (f != NULL) {
		//fread((void *)&header, sizeof(header), 1, f);
			
		// generate data for audio buffer 1
		fread(&Audio_Buffer1,sizeof(int16_t),2048,f);
	
		// Start the audio player
		BSP_AUDIO_OUT_Play((uint16_t *)Audio_Buffer1, BUF_LEN);
			
		while(!feof(f)) {
		
			// generate data for audio buffer 2
			fread(&Audio_Buffer2,sizeof(int16_t),2048,f);
		
			osMessagePut (mid_MsgQueue, 2, 0); // Send Message
			osSemaphoreWait(Sem_id,osWaitForever);
		
			// generate data for audio buffer 1
			fread(&Audio_Buffer1,sizeof(int16_t),2048,f);

			osMessagePut (mid_MsgQueue, 1, 0); // Send Message
			osSemaphoreWait(Sem_id,osWaitForever);
		}
		fclose (f); // close the file
	} // end if file opened
	*/
		
	return;
}

/* User Callbacks: user has to implement these functions in his code if they are needed. */
/* This function is called when the requested data has been completely transferred. */
void    BSP_AUDIO_OUT_TransferComplete_CallBack(void){
	osEvent evt; // receive object
	evt = osMessageGet (mid_MsgQueue, 0);           // get message
	if (evt.status == osEventMessage) {
		
		if (evt.value.v == 1)
			BSP_AUDIO_OUT_ChangeBuffer((uint16_t*)Audio_Buffer1, BUF_LEN);
		
		else if (evt.value.v == 2)
			BSP_AUDIO_OUT_ChangeBuffer((uint16_t*)Audio_Buffer2, BUF_LEN);
	
		osSemaphoreRelease(Sem_id);
	}
	else
		BSP_AUDIO_OUT_ChangeBuffer((uint16_t*)Audio_Buffer1, BUF_LEN);
}

/* This function is called when half of the requested buffer has been transferred. */
void    BSP_AUDIO_OUT_HalfTransfer_CallBack(void){
}

/* This function is called when an Interrupt due to transfer error on or peripheral
   error occurs. */
void    BSP_AUDIO_OUT_Error_CallBack(void){
		while(1){
		}
}