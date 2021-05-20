// Thread.c

#include <cmsis_os.h>                                           // CMSIS RTOS header file
#include "stm32f4xx.h"
#include "my_header.h"

/*----------------------------------------------------------------------------
*      Declaration of all Thread ID's: needed for influencing flags
 *---------------------------------------------------------------------------*/

osThreadId tid_Blink_Bl_Thread; // Declare a threat ID varaible to save ID during initialization     
osThreadId tid_Blink_Or_Thread; // Declare a threat ID varaible to save ID during initialization                                        
osThreadId tid_Button_Thread; // Declare a threat ID varaible to save ID during initialization                                        
osThreadId tid_Tap_Thread; // Declare a threat ID varaible to save ID during initialization                                        
osThreadId tid_Sequence_Thread; // Declare a threat ID varaible to save ID during initialization                                        
osThreadId tid_Beep_Thread; // Declare a threat ID varaible to save ID during initialization                                        



/*----------------------------------------------------------------------------
 *      Thread 1 'Blink_Bl_Thread': Blinks blue LED
 *---------------------------------------------------------------------------*/
 
void Blink_Bl_Thread(void const *argument); // Define structure for osThreadDef-function                                                                
osThreadDef(Blink_Bl_Thread, osPriorityNormal, 1, 0); // Define thread including number of instances and priority               

int Init_Blink_Bl_Thread(void){ // Definition of function to initialize the Thread

  tid_Blink_Bl_Thread = osThreadCreate(osThread(Blink_Bl_Thread), NULL); // Initialise Thread with RTOS-function
  if(!tid_Blink_Bl_Thread) return(-1); // test if initilization was successful. if not return -1
  
  return(0); // Initilization worked => return 0
}

void Blink_Bl_Thread (void const *argument){ // Definition of the actual thread function
	
	osSignalClear(tid_Blink_Bl_Thread,0x01); // set flag for given thread as a signal -> stop at next wait
	
  while(1){ // continuing loop
		
		osSignalWait(0x01,osWaitForever); // Wait until given Flag for this thread is set in Sequence_Thread
		osSignalSet(tid_Blink_Bl_Thread,0x01); // set flag for given thread as a signal -> continue at next wait
		
    Set_State_Bl_LED(0); // Turn off blue LED -> Is always startet by Sequence_Thread with turning off LED
		
		osDelay(100); // wait 100 ms
		
		Set_State_Bl_LED(1); // Turn on blue LED -> Is always stopped by Sequence_Thread after turning on LED
		
		osDelay(100); // wait 100 ms
  }
}


/*----------------------------------------------------------------------------
 *      Thread 2 'Blink_Or_Thread': Blinks orange LED
 *---------------------------------------------------------------------------*/
 
void Blink_Or_Thread(void const *argument); // Define structure for osThreadDef-function                             
osThreadDef(Blink_Or_Thread, osPriorityNormal, 1, 0); // Define thread including number of instances and priority               

int Init_Blink_Or_Thread(void){ // Definition of function to initialize the Thread

  tid_Blink_Or_Thread = osThreadCreate(osThread(Blink_Or_Thread), NULL); // Initialise Thread with RTOS-function
  if(!tid_Blink_Or_Thread) return(-1); // test if initilization was successful. if not return -1
  
  return(0); // Initilization worked => return 0
}

void Blink_Or_Thread (void const *argument){ // Definition of the actual thread function
	
	osSignalClear(tid_Blink_Or_Thread,0x01); // set flag for given thread as a signal -> stop at next wait
	
  while(1){ // continuing loop
		
		osSignalWait(0x01,osWaitForever); // Wait until given Flag for this thread is set in Sequence_Thread
		osSignalSet(tid_Blink_Or_Thread,0x01); // set flag for given thread as a signal -> continue at next wait
		
		Set_State_Or_LED(1); // Turn on orange LED -> Is always startet by Sequence_Thread with turning on LED
		
		osDelay(300); // wait 300 ms
		
		Set_State_Or_LED(0); // Turn off orange LED -> Is always stopped by Sequence_Thread after turning off LED
		
		osDelay(300); // wait 300 ms
  }
}


/*----------------------------------------------------------------------------
 *      Thread 3 'Button_Thread': Tests for pressing of user buttton B1
 *---------------------------------------------------------------------------*/
 
void Button_Thread(void const *argument); // Define structure for osThreadDef-function                             
osThreadDef(Button_Thread, osPriorityNormal, 1, 0); // Define thread including number of instances and priority               

int Init_Button_Thread(void){ // Definition of function to initialize the Thread

  tid_Button_Thread = osThreadCreate(osThread(Button_Thread), NULL); // Initialise Thread with RTOS-function
  if(!tid_Button_Thread) return(-1); // test if initilization was successful. if not return -1
  
  return(0); // Initilization worked => return 0
}

void Button_Thread (void const *argument){ // Definition of the actual thread function
  
	osSignalSet(tid_Button_Thread,0x01); // set flag for given thread as a signal -> start looking for button presses from reset
	
  while(1){ // continuing loop
		
		osSignalWait(0x01,osWaitForever); // Wait until given Flag for this thread is set in Sequence_Thread -> look for new button presses
		
		while(!Get_State_B1()){ // wait till button is pressed
			osThreadYield(); // Ensure to call other thread if available
		}
		osSignalSet(tid_Sequence_Thread,0x01); // allow Sequence_Thread to enter crossing sequence
  }
}


/*----------------------------------------------------------------------------
 *      Thread 4 'Tap_Thread': Detect tapping on the board
 *---------------------------------------------------------------------------*/
 
void Tap_Thread(void const* argument); // Define structure for osThreadDef-function                             
osThreadDef(Tap_Thread, osPriorityNormal, 1, 0); // Define thread including number of instances and priority               

int Init_Tap_Thread(SPI_HandleTypeDef* SPI_Params_adr){ // Definition of function to initialize the Thread

  tid_Tap_Thread = osThreadCreate(osThread(Tap_Thread), SPI_Params_adr); // Initialise Thread with RTOS-function
  if(!tid_Tap_Thread) return(-1); // test if initilization was successful. if not return -1
  
  return(0); // Initilization worked => return 0
}

void Tap_Thread (void const* argument){ // Definition of the actual thread function
  
	// Variables declaration
	SPI_HandleTypeDef* SPI_Params_adr; // Declares the variable to store SPI parameters pointer
	uint8_t XYZ_Reg_H; // Declares the variable to store MS 8-bits of a measurement in
	uint8_t XYZ_Reg_L; // Declares the variable to store the LS 8-bits of a measurement in
	int16_t X_Reg_old; // Declares the variable to store the old x-axis value
	int16_t X_Reg_new; // Declares the variable to store the new x-axis value
	int16_t Step_Min = 3000; // Declares the variable to store the minimal absolute Change in value to detect a tap 
	
	SPI_Params_adr = (SPI_HandleTypeDef*) argument; // Force type conversion for given arguments (SPI parameters pointer)
	
	// Get reference measurement for first X_Reg_old
	// Read 8 highest output bits of x-axis measurement value
	XYZ_Reg_H = Read_LIS3DSH(0x29, SPI_Params_adr); // read from LIS3DSH with subfunction
	// Read 8 highest output bits of x-axis measurement value
	XYZ_Reg_L = Read_LIS3DSH(0x28, SPI_Params_adr); // read from LIS3DSH with subfunction
	// set both 8 bits together for signed value
	X_Reg_old = XYZ_Reg_H<<8 | XYZ_Reg_L;
	
	
	osSignalClear(tid_Tap_Thread,0x01); // clear flag for given thread as a signal -> stop looking for tapping from reset
	
  while(1){ // continuing loop
		
		osSignalWait(0x01,osWaitForever); // Wait until given Flag for this thread is set in Sequence_Thread -> look for new button presses
		osSignalSet(tid_Tap_Thread,0x01); // continue at next wait
		
		if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0)==SET){ // Checks if the interrupt line is set -> new measurement
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0); // reset interrupt line to reuse it
			

			// Read 8 highest output bits of x-axis measurement value
			XYZ_Reg_H = Read_LIS3DSH(0x29, SPI_Params_adr); // read from LIS3DSH with subfunction
			// Read 8 highest output bits of x-axis measurement value
			XYZ_Reg_L = Read_LIS3DSH(0x28, SPI_Params_adr); // read from LIS3DSH with subfunction
			// set both 8 bits together for signed value
			X_Reg_new = XYZ_Reg_H<<8 | XYZ_Reg_L;
			
			if((X_Reg_new-X_Reg_old)<(-Step_Min) || (X_Reg_new-X_Reg_old)>Step_Min){ // Test change since last measurement for tap
				osSignalSet(tid_Sequence_Thread,0x02); // found tap -> signal to Sequence_Thread
				// make Tap detection visible
				//Set_State_Gn_LED(1); // turn on the green LED
				//osDelay(1000);
				//Set_State_Gn_LED(0); // turn on the green LED
			}
			X_Reg_old = X_Reg_new; // Set current value as Reference for next measurement
			
		}

		osThreadYield(); // Ensure to call other thread if available
  }
}


/*----------------------------------------------------------------------------
 *      Thread 5 'Sequence_Thread': Initilisation and crossing sequence control
 *---------------------------------------------------------------------------*/
 
void Sequence_Thread(void const *argument); // Define structure for osThreadDef-function                             
osThreadDef(Sequence_Thread, osPriorityNormal, 1, 0); // Define thread including number of instances and priority               

int Init_Sequence_Thread(void){ // Definition of function to initialize the Thread

  tid_Sequence_Thread = osThreadCreate(osThread(Sequence_Thread), NULL); // Initialise Thread with RTOS-function
  if(!tid_Sequence_Thread) return(-1); // test if initilization was successful. if not return -1
  
  return(0); // Initilization worked => return 0
}

void Sequence_Thread (void const *argument){ // Definition of the actual thread function

	// Variables declaration
	uint8_t fromReset = 1; // flag indicating coming from reset => no 15s wait
	
	// Initialise
	Set_State_Bl_LED(1); // Turn blue LED on
	Set_State_Gn_LED(1); // Turn green LED on
	osSignalClear(tid_Sequence_Thread,0x01); // set flag for given thread as a signal -> stop at next wait until user button is pressed
	osSignalClear(tid_Sequence_Thread,0x02); // set flag for given thread as a signal -> not Tap deceted -> stop at next wait for 10s	
  
	while(1){ // continuing loop
		
		osSignalWait(0x01,osWaitForever); // Wait until given Flag for this thread is set in Button_Thread -> start crossing sequence
		while( (!Get_State_TIM2()) && !fromReset ){}// wait until 15 seconds have elapsed since last crossing sequence or skip when coming from reset
		
		if(fromReset){
			fromReset = 0; // skip while only first time from reset
		}
		
		// crossing sequence
		Set_State_Gn_LED(0); // Turn green LED off
		Set_State_Or_LED(1); // Turn orange LED on
		
		osDelay(2000); // wait 2s
			
		Set_State_Or_LED(0); // Turn orange LED off
		Set_State_Rd_LED(1); // Turn red LED on
		osSignalSet(tid_Tap_Thread,0x01); // Allow Tap_Thread to start -> start looking for a Tap
			
		osDelay(1000); // wait 1s
		
		osSignalSet(tid_Beep_Thread,0x01); // Allow Beep_Thread to start -> start beeping
		osSignalSet(tid_Blink_Bl_Thread,0x01); // Allow Blink_Bl_Thread to start -> blink blue LED
		
		osDelay(15000); // wait 15s
			
		osSignalClear(tid_Tap_Thread,0x01); // Stop Tap_Thread -> stop looking for a tap
		
		osSignalWait(0x02,10000); // If flag in Tap_Thread was set don't wait. If not wait 10s
			
		Set_State_Rd_LED(0); // Turn red LED off
		osSignalClear(tid_Beep_Thread,0x01); // stop Beep_Thread -> stop beeping
		osSignalSet(tid_Blink_Or_Thread,0x01); // Allow Blink_Or_Thread to start -> blink orange LED
		osSignalSet(tid_Button_Thread,0x01); // Allow Button_Thread to look for new button presses
			
		osDelay(5000); // wait 5s
			
		osSignalClear(tid_Blink_Or_Thread,0x01); // stop Blink_Or_Thread -> stop blink orange LED, let finish last cycle
		osSignalClear(tid_Blink_Bl_Thread,0x01); // stop Blink_Bl_Thread -> stop blink blue LED, let finish last cycle
		Set_State_Gn_LED(1); // Turn green LED on
		Enable_TIM2(); // Start timer 2 (15s)
		
  }
}

/*----------------------------------------------------------------------------
 *      Thread 6 'Beep_Thread': Emit beeping sound through onboard DAC audio driver
 *---------------------------------------------------------------------------*/
 
void Beep_Thread(void const* argument); // Define structure for osThreadDef-function                             
osThreadDef(Beep_Thread, osPriorityNormal, 1, 0); // Define thread including number of instances and priority               

int Init_Beep_Thread(I2S_HandleTypeDef* I2S_Params_adr){ // Definition of function to initialize the Thread

  tid_Beep_Thread = osThreadCreate(osThread(Beep_Thread), I2S_Params_adr); // Initialise Thread with RTOS-function
  if(!tid_Beep_Thread) return(-1); // test if initilization was successful. if not return -1
  
  return(0); // Initilization worked => return 0
}

void Beep_Thread (void const* argument){ // Definition of the actual thread function
  
	// Variables declaration
	uint32_t ii; // iteration variable for-loop
	I2S_HandleTypeDef* SPI_Params_adr; // Declares the variable to store I2S parameters pointer
	uint16_t I2S_Data[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Declares data to be send over I2S
	HAL_StatusTypeDef I2S_State; // declare State handle for I2S
	
	SPI_Params_adr = (I2S_HandleTypeDef*) argument; // Force type conversion for given arguments (I2S parameters pointer)
	
	osSignalClear(tid_Beep_Thread,0x01); // clear flag for given thread as a signal -> stop from reset
	
  while(1){ // continuing loop
		
		osSignalWait(0x01,osWaitForever); // Wait until given Flag for this thread is set in Sequence_Thread -> look for new button presses
		osSignalSet(tid_Beep_Thread,0x01); // continue at next wait
		
		for(ii=0; ii<100; ii++){ // make 100 transmissions of ten 16 Bit values
			I2S_State = HAL_I2S_Transmit(SPI_Params_adr, I2S_Data, 10, 1000); // send empty data over I2S to activate MCLK => start beep
		}	
		osThreadYield(); // Ensure to call other thread if available
  }
}
