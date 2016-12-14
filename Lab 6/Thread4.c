

#include "cmsis_os.h"  // CMSIS RTOS header file

#include "Board_LED.h"

#include "UART_driver.h" // Prototypes for Uart_Init and others

#include "stdint.h"                     // data type definitions

#include "stdio.h"                      // file I/O functions

#include "rl_usb.h"                     // Keil.MDK-Pro::USB:CORE

#include "rl_fs.h"                      // Keil.MDK-Pro::File System:CORE

#include "stm32f4xx_hal.h"

#include "stm32f4_discovery.h"

#include "stm32f4_discovery_audio.h"

#include <stdio.h>

#include <stdlib.h>



// Definitions and Extern

extern ARM_DRIVER_USART Driver_USART4;

//States

#define	STOPPED	2

#define	PLAYING	0

#define	PAUSED	1



// Actions - Codes for sending actions throughout the threads

#define	NOACTION	3

#define	PLAYCMD	4

#define	PAUSECMD	1

#define	ACCESSFILE	2

#define	UPDATEFILE	5



//Mail and Message Objects

#define MSGQUEUE_OBJECTS      1  



// Events - These events are signals sent to the Control Thread that will initiate

// a state change and an eventual action

#define GUIPLAY	0x30

#define GUIPAUSE 0x31

#define	GUIFILE 0x32

#define GUIFNAME 0x33

#define	SONGEND 0x34



// Variables - Maximum character length of the file name

#define MAXCHAR	25



//// Prototypes

void Thread_ReceiveGUI (void const *argument);  // The thread that receives the input from the gui and sends it to control

void Thread_Control (void const *argument); // Updates the state and sends actions to the appropriate thread

void Thread_Access_USB (void const *argument); // Accesses the file names on the USB and sends them to the GUI

void Thread_AudioPlay (void const *argument); // Receives a file name and plays the corresponding file

int State_Mac(uint16_t input); // Takes an event and updates the state



// Global Variables

int state=STOPPED; // State variable initialized to STOPPED state

char currentFileName[MAXCHAR]; // Global file name variable

uint16_t input; // Global event variable that translates actions from thread to thread

int Audio_first_time = 1; // Ensures the Audio player is not initialized more than once

int newFile = 0;

uint8_t buf2use = 2; //Buffer Variable for Audio Buffer exchange



// Definitions and ID's : Thread, MsgQueues, Semaphores

osMessageQId Control_Queue;                                // Control Thread Message Queue

osMessageQDef (ControlQueue, MSGQUEUE_OBJECTS, input);     // Message queue object



osMessageQId mid_MsqQueue;

osMessageQDef (MsgQueue, 1, uint32_t);



osMessageQId Pause_Queue;                                        // Pause Message Queue

osMessageQDef (Pause, MSGQUEUE_OBJECTS, input);     // Message queue object



osMessageQId AudioPlay_Queue;                                        // Audio Play Message Queue

osMessageQDef (AudioPlay, MSGQUEUE_OBJECTS, input);     // Message queue object



osMessageQId FileName_Queue;                                        // File Name Access Message Queue

osMessageQDef (FileName, MSGQUEUE_OBJECTS, char*);     // Message queue object



osMessageQId Access_Queue;                                        // Message queue id

osMessageQDef (AccessQueue, MSGQUEUE_OBJECTS, input);     // Message queue object



osThreadId tid_Thread_ReceiveGUI;                               // thread id

osThreadDef (Thread_ReceiveGUI, osPriorityNormal, 1, 0);         // thread object 



osThreadId tid_Thread_Control;                                          // thread id

osThreadDef (Thread_Control, osPriorityNormal, 1, 0);                   // thread object



osThreadId tid_Thread_Access_USB;                                          // thread id

osThreadDef (Thread_Access_USB, osPriorityNormal, 1, 0);                   // thread object



osThreadId tid_Thread_AudioPlay;                                          // thread id

osThreadDef (Thread_AudioPlay, osPriorityNormal, 1, 0);                   // thread object



//Semaphores

osSemaphoreDef(Sem);

osSemaphoreId(Sem_id);



//Initializes all message Queues, Threads, and Semaphores

int Init_Thread (void) {

	

	UART_Init();	 

	LED_Initialize();

	LED_On(2);

	UART_send("7\n",2); // updates Stopped state on GUI	

	UART_send("2\n",2); // updates Reset action on GUI

	currentFileName[1] =0;

	

	Control_Queue = osMessageCreate (osMessageQ(ControlQueue), NULL);      // create mail queue

  if (!Control_Queue) {

    return(-1); // Mail Queue object not created, handle failure

  }  

	AudioPlay_Queue = osMessageCreate (osMessageQ(AudioPlay), NULL);      // create mail queue

  if (!AudioPlay_Queue) {

    return(-1); // Mail Queue object not created, handle failure

  }  

	

		Pause_Queue = osMessageCreate (osMessageQ(Pause), NULL);      // create mail queue

  if (!Pause_Queue) {

    return(-1); // Mail Queue object not created, handle failure

  }  

	

		FileName_Queue = osMessageCreate (osMessageQ(FileName), NULL);      // create mail queue

  if (!FileName_Queue) {

    return(-1); // Mail Queue object not created, handle failure

  }  

	

	Access_Queue = osMessageCreate (osMessageQ(AccessQueue), NULL);      // create mail queue

  if (!Access_Queue) {

    return(-1); // Mail Queue object not created, handle failure

  } 

	

	mid_MsqQueue = osMessageCreate(osMessageQ(MsgQueue), NULL);

	

	Sem_id = osSemaphoreCreate(osSemaphore(Sem), 0);

	

	tid_Thread_AudioPlay = osThreadCreate (osThread(Thread_AudioPlay), NULL);

  if (!tid_Thread_AudioPlay) return(-1);

	

	tid_Thread_Control = osThreadCreate (osThread(Thread_Control), NULL);

	if (!tid_Thread_Control) return(-1);

	

	tid_Thread_ReceiveGUI = osThreadCreate (osThread(Thread_ReceiveGUI), NULL);

  if (!tid_Thread_ReceiveGUI) return(-1);

	

  tid_Thread_Access_USB = osThreadCreate (osThread(Thread_Access_USB), NULL);

  if (!tid_Thread_Access_USB) return(-1);

	

  return(0);

}



/*----------------------------------------------------------------------------

*      Thread 1 'ReceiveGUI': 

*

*	This Thread is the control thread for the RTOS, taking the input from the UI

* and sending data to the state machine, and then sending control information to

* the appropriate threads. 

*

 *---------------------------------------------------------------------------*/

void Thread_ReceiveGUI (void const *argument) {

	

  while (1) {

    // Creates variables to hold character inputs from GUI

		char r_data[2]={0,0};

		char r_string[15];

		int i;

		for(i=0;i<15;i++) // initializes file name string to null characters

			r_string[i] = 0;

		

		while (1) {

			UART_receive(r_data, 1); //Reads event from GUI

		

			if(!strcmp(r_data,"0")){

				input = (uint16_t) r_data[0];

				osMessagePut(Control_Queue, input, osWaitForever);

			}

			else if(!strcmp(r_data,"1")){

				input = (uint16_t) r_data[0];

				osMessagePut(Control_Queue, input, osWaitForever);

			}

			else if(!strcmp(r_data,"2")){

				input = (uint16_t) r_data[0];

				osMessagePut(Control_Queue, input, osWaitForever);

			}

			else if (!strcmp(r_data,"3")){

				input = (uint16_t) r_data[0];

				osMessagePut(Control_Queue, input, osWaitForever);

				UART_receivestring(r_string, MAXCHAR);

				for(i=0;i<MAXCHAR;i++){

					currentFileName[i] = r_string[i];

				}

				UART_send("8\n", 2);

				UART_send(r_string, MAXCHAR);

				UART_send("\n",1);

			} 

		} // end subWhile

	} // end while

}



// The State Machine that handles the input, returns an action, and updates the states accordingly.

int State_Mac(uint16_t input) {

	uint16_t action; 

		if(input == GUIPLAY && state == STOPPED && currentFileName[1] !=0 ) {

				action = PLAYCMD;

				state = PLAYING;

				LED_On(3);

				LED_Off(1);

				LED_Off(2);

				UART_send("0\n",2);

		}

		else if (input ==GUIPLAY && state == PAUSED) {

				action = PLAYCMD;

				state = PLAYING;

				LED_On(3);

				LED_Off(1);

				LED_Off(2);

				UART_send("0\n",2);

		}

		else if(input == GUIPAUSE && state == PLAYING) {

				action = PAUSECMD;

				state = PAUSED;

				LED_On(1);

				LED_Off(2);

				LED_Off(3);			

				UART_send("1\n",2);

		}

		else if(input == GUIFILE) {

				action = ACCESSFILE;

		}

		else if(input ==GUIFNAME) {

				action = UPDATEFILE;

		}

		else if(input == SONGEND) {

				action = NOACTION;

				state = STOPPED;

				UART_send("2\n",2);

				LED_On(2);

				LED_Off(1);

				LED_Off(3);

		}

		else

			action = NOACTION;

			

	return action;

}



/*----------------------------------------------------------------------------

*      Thread  'Control': 

*

*	This Thread is the control thread for the RTOS, taking the input from the UI

* and sending data to the state machine, and then sending control information to

* the appropriate threads. 

*

 *---------------------------------------------------------------------------*/

void Thread_Control (void const *argument) {

	osEvent event;

	uint16_t input;

	uint16_t action;

	while(1) {

		event = osMessageGet(Control_Queue, osWaitForever);

		if(event.status == osEventMessage) {

			input = event.value.v;

			action = State_Mac(input);

			if (action == NOACTION) 

				UART_send("3\n",2);

			else if (action == PLAYCMD) {

				UART_send("4\n",2);

				osMessagePut(AudioPlay_Queue, action, osWaitForever);

			}

			else if (action == PAUSECMD) {

				UART_send("5\n",2);

				osMessagePut(Pause_Queue, action, osWaitForever);

			}

			else if (action == ACCESSFILE){

				UART_send("6\n",2);

				osMessagePut(Access_Queue,action,osWaitForever); 

			}

			else if (action == UPDATEFILE) {

				osMessagePut(Pause_Queue, action, osWaitForever);

			}

		}

	}

}



/*----------------------------------------------------------------------------

*      Thread  'USB_File Access': 

*

*	This Thread accesses the files on the OTG USB, and returns them to a list box

* on the GUI 

*

*----------------------------------------------------------------------------*/



void Thread_Access_USB (void const *argument) {

	usbStatus ustatus; // USB driver status variable

	uint8_t drivenum = 0; // Using U0: drive number

	char *drive_name = "U0:"; // USB drive name

	fsStatus fstatus; // file system status variable

	fsFileInfo info;

	info.fileID = 0;

	osEvent event;

	

	while(1) {

		event = osMessageGet(Access_Queue, osWaitForever);

		if(event.status == osEventMessage) {

			LED_On(0);

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

					}	 	// end if

			// Mount the drive

					fstatus = fmount (drive_name);

					if (fstatus != fsOK){

				// handle the error, fmount didn't work

					} // end if 

			// look in the USB to find file names and pring to UART

					while(ffind("U0:*.*", &info) == fsOK){

						UART_send(info.name,strlen(info.name));

						UART_send("\n",1);

					}	

				}			// end if USBH_Initialize

		LED_Off(0);

		}

	}

}



/*----------------------------------------------------------------------------

*      Thread: "Audio_Player"

* The Audio Player Thread accesses the USB and retrieves the file that has been 

* stored in the global file name variable. It then sends the data via two audio

* buffers to the DMA and ultimately the CODEC for output to 

*

*

 *---------------------------------------------------------------------------*/

 #define NUM_CHAN	2 // number of audio channels

 #define NUM_POINTS 1024 // number of points per channel

 #define BUF_LEN NUM_CHAN*NUM_POINTS // length of the audio buffer



// pointer to file type for files on USB device

FILE *f;



//The two audio buffers used to send data to the codec

int16_t Audio_Buffer1[BUF_LEN];

int16_t Audio_Buffer2[BUF_LEN];



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



void Thread_AudioPlay (void const *argument) {

	usbStatus ustatus; // USB driver status variable

	uint8_t drivenum = 0; // Using U0: drive number

	char *drive_name = "U0:"; // USB drive name

	fsStatus fstatus; // file system status variable

	WAVHEADER header;

	size_t rd;

	uint32_t i;

	static uint8_t rtrn = 0;

	uint8_t rdnum = 1; // read buffer number

	osEvent event1;

	osEvent event2;

	char localFileName[MAXCHAR];



	while(1){

		event1 = osMessageGet(AudioPlay_Queue, osWaitForever);

		if(event1.value.v == PLAYCMD && currentFileName[1] != 0) {

			f = fopen (currentFileName,"r");// open a file on the USB device

			if (f != NULL) {

				fread((void *)&header, sizeof(header), 1, f);

				newFile = 0;

				for(i=0;i<MAXCHAR;i++)

					localFileName[i] = currentFileName[i];

			}



	// initialize the audio output

		if(Audio_first_time==1){

		rtrn = BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, 0x46, header.sample_rate);

		if (rtrn != AUDIO_OK)return;

			Audio_first_time = 0;

		}

		BSP_AUDIO_OUT_SetMute(AUDIO_MUTE_OFF);

		fread((void *)Audio_Buffer1, 2*BUF_LEN, 1, f);

	

		BSP_AUDIO_OUT_Play((uint16_t *)Audio_Buffer1, 2*BUF_LEN);

	

	// generate data for the audio buffer

		while(!feof(f)){

		//Do another wait for message, do with 0 wait time, check if valid message

		event1 = osMessageGet(Pause_Queue, 0);

		if(event1.value.v == PAUSECMD) {

			BSP_AUDIO_OUT_SetMute(AUDIO_MUTE_ON);

			//BSP_AUDIO_OUT_Pause();

			event1 = osMessageGet(AudioPlay_Queue, osWaitForever);

			if(event1.value.v == PLAYCMD && !newFile){

				BSP_AUDIO_OUT_SetMute(AUDIO_MUTE_OFF);

				BSP_AUDIO_OUT_Play((uint16_t *)Audio_Buffer1, 2*BUF_LEN);

				//BSP_AUDIO_OUT_Resume();

			}

//			else {

//				fclose(f);

//				f = fopen (currentFileName,"r");// open a file on the USB device

//				if (f != NULL) {

//				fread((void *)&header, sizeof(header), 1, f);

//				newFile = 0;

//				for(i=0;i<MAXCHAR;i++)

//					localFileName[i] = currentFileName[i];

//				}

//				fread((void *)Audio_Buffer1, BUF_LEN, 1, f);

//				BSP_AUDIO_OUT_Play((uint16_t *)Audio_Buffer1, 2*BUF_LEN);

//			}

		}

//		else if(event1.value.v == UPDATEFILE )

//			newFile = !strcmp(currentFileName,localFileName);

			

//		else if (event.value.v == NEWFILEPLAY)

//			break;

			if(buf2use == 1){

				fread((void *)Audio_Buffer1, 2*BUF_LEN, 1, f);

				osMessagePut(mid_MsqQueue, buf2use, osWaitForever);

				buf2use = 2;

			}

			else {

				fread((void *)Audio_Buffer2, 2*BUF_LEN, 1, f);

				osMessagePut(mid_MsqQueue, buf2use, osWaitForever);

				buf2use = 1;

			}

	

			osSemaphoreWait(Sem_id, osWaitForever);

		}

		BSP_AUDIO_OUT_SetMute(AUDIO_MUTE_ON);

		fclose(f);

		osMessagePut(Control_Queue, 0x34,0); // Send message to Control signifying end of song. 

		}

	}

}





/* User Callbacks: user has to implement these functions in his code if they are needed. */

/* This function is called when the requested data has been completely transferred. */

void  BSP_AUDIO_OUT_TransferComplete_CallBack(void){

	

	osEvent event;

	event = osMessageGet(mid_MsqQueue, 0);

	if(event.status == osEventMessage){

		osSemaphoreRelease(Sem_id);

		if(event.value.v == 1){

			BSP_AUDIO_OUT_ChangeBuffer((uint16_t*)Audio_Buffer1, BUF_LEN);

		}

		else{

			BSP_AUDIO_OUT_ChangeBuffer((uint16_t*)Audio_Buffer2, BUF_LEN);

		}

	}	 

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

	