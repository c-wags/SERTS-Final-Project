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
#include <stdio.h>

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

// pointer to file type for files on USB device
FILE *f;

void Thread (void const *argument);                             // thread function
osThreadId tid_Thread;                                          // thread id
osThreadDef (Thread, osPriorityNormal, 1, 0); 
void SDCO (void const *argument);                             // thread function
osThreadId tid_SDCO;                                          // thread id
osThreadDef (SDCO, osPriorityNormal, 1, 0); 
void Indexing (void const *argument);                             // thread function
osThreadId tid_Indexing;                                          // thread id
osThreadDef (Indexing, osPriorityNormal, 1, 0); 
void Playing (void const *argument);                             // thread function
osThreadId tid_Playing;                                          // thread id
osThreadDef (Playing, osPriorityNormal, 1, 0);                   // thread object

// Variable to show what state we're in: 0=indexing, 1=playing
int state = 0;

void Init_Thread (void) {

	LED_Initialize();
	UART_Init();
	
	tid_Thread = osThreadCreate (osThread(Thread), NULL);
  if (!tid_Thread) return;
	
	tid_SDCO = osThreadCreate (osThread(SDCO), NULL);
  if (!tid_SDCO) return;
	
	tid_Indexing = osThreadCreate (osThread(Indexing), NULL);
  if (!tid_Indexing) return;
	
	tid_Playing = osThreadCreate (osThread(Playing), NULL);
  if (!tid_Playing) return;
	
	LED_On(0);
	
}

void Thread (void const *argument) {
	usbStatus ustatus; // USB driver status variable
	uint8_t drivenum = 0; // Using U0: drive number
	char *drive_name = "U0:"; // USB drive name
	fsStatus fstatus; // file system status variable
	WAVHEADER header;
	size_t rd;
	uint32_t i;
	static uint8_t rtrn = 0;
	uint8_t rdnum = 1; // read buffer number
	
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
		f = fopen ("Test.wav","r");// open a file on the USB device
		if (f != NULL) {
			fread((void *)&header, sizeof(header), 1, f);
			fclose (f); // close the file
		} // end if file opened
	} // end if USBH_Initialize

} // end Thread

void SDCO (void const *argument) {
	
}

void Indexing (void const *argument) {
	// Switch to Playing when play button pressed
}

void Playing (void const *argument) {
	// Switch to Indexing pause button pressed or song ends
}
