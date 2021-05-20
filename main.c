// main.c

/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function
 *---------------------------------------------------------------------------*/

#define osObjectsPublic // define objects in main module
#include "osObjects.h" // RTOS object definitions
#include "stm32f4xx.h"
#include "my_header.h"
#include "Thread.h"


int main(void){
	
	SPI_HandleTypeDef SPI_Params; // create structure for SPI communication Paramaters
	I2S_HandleTypeDef I2S_Params; // create structure for I2S communication Paramaters
	
	
	
	HAL_Init(); // Initialise Hal driver
	
	Init_CS43L22(&I2S_Params); // initialise I2C and I2S communication to CS43L22. Set up beep function.
	
	Init_LIS3DSH(&SPI_Params); // initialise SPI communication (including interupt) settings and receive Paramaters
	
	// Write value to control register 4 on the LIS3DSH
	// set x and y axis, continious update and ouput data rate (400 Hz)
	Write_LIS3DSH(0x20, 0x71, &SPI_Params); // write to LIS3DSH with subfunction
	
	Init_TIM2(150000); // Initialize timer 2 with a duration of 15s
	LED_Init(); // Initialize all LEDs
	B1_Init(); // Initialize user Button B1
	
	osKernelInitialize (); // initialize CMSIS-RTOS
	
	Init_Blink_Bl_Thread(); // Initialize Blink_Bl_Thread
	Init_Blink_Or_Thread(); // Initialize Blink_Or_Thread
	Init_Button_Thread(); // Initialize Button_Thread
	Init_Tap_Thread(&SPI_Params); // Initialize Tap_Thread
	Init_Sequence_Thread(); // Initialize Blink_LED_Thread
	Init_Beep_Thread(&I2S_Params); // Initialize Beep_Thread
	
	osKernelStart (); // Start thread execution
	
	while(1){}; // keep main function running
}
