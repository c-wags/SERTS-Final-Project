#include "cmsis_os.h"  // CMSIS RTOS header file
#include "Board_LED.h"
#include "UART_driver.h"
#include "stdint.h"                     // data type definitions
#include "stdio.h"                      // file I/O functions
#include "rl_usb.h"                     // Keil.MDK-Pro::USB:CORE
#include "rl_fs.h"                      // Keil.MDK-Pro::File System:CORE
#include "string.h" // in order to clear the buffer
#include "stdlib.h"

void Thread (void const *argument);                             // thread function
osThreadId tid_Thread;                                          // thread id
osThreadDef (Thread, osPriorityNormal, 1, 0);                   // thread object

void timeDelay (int length) {
	int x, y;
	for (x = 1; x <= length; x++)
		for (y = 1; y <= length; y++)
	  {}
}

int Init_Thread (void) {

	LED_Initialize();
	UART_Init();
  tid_Thread = osThreadCreate (osThread(Thread), NULL);
  if (!tid_Thread) return(-1);
  
  return(0);
}

void Thread (void const *argument) {
	usbStatus ustatus; // USB driver status variable
	uint8_t drivenum = 0; // Using U0: drive number
	char *drive_name = "U0:"; // USB drive name
	fsStatus fstatus; // file system status variable
	static FILE *f;
	
	/* USART Driver */
	extern ARM_DRIVER_USART Driver_USART4;
	
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
		char buffer[50];
		while (ffind ("*", &info) == fsOK) {     // find whatever is in any drive"
			sprintf(buffer,"\r\n%s",info.name);
			Driver_USART4.Send(buffer,50); // send to UART
			timeDelay(2500); // delay the program to allow time to interface with Putty
			memset(buffer, 0, 50 * (sizeof buffer[0]) ); // clear buffer
		}
		
	} // end if USBH_Initialize
	LED_Off(0);
}
