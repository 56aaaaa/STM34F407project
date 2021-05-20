// my_header.c

#include "stm32f4xx.h"
#include "my_header.h"


// Definition for the function to initialises all LEDs
void LED_Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable Port D clock
	GPIOD->MODER |= GPIO_MODER_MODER15_0; // Port D.15 output - blue LED
	GPIOD->MODER |= GPIO_MODER_MODER14_0; // Port D.14 output - red LED
	GPIOD->MODER |= GPIO_MODER_MODER13_0; // Port D.13 output - orange LED
	GPIOD->MODER |= GPIO_MODER_MODER12_0; // Port D.12 output - green LED
}


// Definition for the function to set state of green LED
// state = 1 to turn LED on; state = 0 to turn LED off
void Set_State_Gn_LED(uint8_t state){
	if(state){
		GPIOD->BSRR = 1<<12; // Turn on the green LED
	}
	else{
		GPIOD->BSRR = 1<<(12+16); // Turn off the green LED
	}
}


// Definition for the function to set state of orange LED
// state = 1 to turn LED on; state = 0 to turn LED off
void Set_State_Or_LED(uint8_t state){
	if(state){
		GPIOD->BSRR = 1<<13; // Turn on the orange LED
	}
	else{
		GPIOD->BSRR = 1<<(13+16); // Turn off the orange LED
	}
}


// Definition for the function to set state of red LED
// state = 1 to turn LED on; state = 0 to turn LED off
void Set_State_Rd_LED(uint8_t state){
	if(state){
		GPIOD->BSRR = 1<<14; // Turn on the red LED
	}
	else{
		GPIOD->BSRR = 1<<(14+16); // Turn off the red LED
	}
}


// Definition for the function to set state of blue LED
// state = 1 to turn LED on; state = 0 to turn LED off
void Set_State_Bl_LED(uint8_t state){
	if(state){
		GPIOD->BSRR = 1<<15; // Turn on the blue LED
	}
	else{
		GPIOD->BSRR = 1<<(15+16); // Turn off the blue LED
	}
}


// Definition for the function to initialise user Button B1
void B1_Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable GPIOA clock
	// GPIOA is set to float input from reset state
	// Pull-Down resistor as real resistor on board
	GPIOA->MODER &= ~GPIO_MODER_MODER0; // ensure beeing set to 00 -> input
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR0; // ensure beeing set to 00 -> no pull-up no pull-down
}


// Definition for the function giving state of user button B1
// returns: 1 when pressed; 0 when not pressed
int Get_State_B1(void){
return (GPIOA->IDR & 0x00000001);
};


// Definition for the function initialising timing via timer 2
// set_Time is time in multiples of 100µs after which timer flag is set
void Init_TIM2(uint32_t set_Time){
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable clock of timer 2
	TIM2->CR1 &= ~0x00000016; // define as upcounter
	TIM2->CR1 |= 0x00000008; // set single pulse mode (no automatic restart)
	TIM2->PSC = 8400-1; // set prescaler value -> timer clock 10kHz for internal oscillator
	TIM2->ARR = (set_Time)-1; // set overflow to (set_Time)-1 steps
	TIM2->EGR = 1; // re-initalise timer 2
}


// Definition for the function signaling state of timer 2
// returns 1 if time has elapsed
// returns 0 if time has not yet elapsed
uint8_t Get_State_TIM2(void){
	return(TIM2->SR&0x0001);
}


// Definition for the function to enable timer 2 and reset overflow flag
void Enable_TIM2(void){
	TIM2->SR &= ~0x0001; //Reset UIF (update interrupt flag) -> reset timer 2 state	
	TIM2->CR1 |= 0x00000001; // enable (start) timer 2
}


// Definition for the function to intialise communication with LIS3DSH over SPI including interupt settings
// SPI_Params_adr: Pointer to Structure of SPI Parameters
void Init_LIS3DSH(SPI_HandleTypeDef* SPI_Params_adr){
	
	uint8_t send_data ; // Variable for data to be send to configure interupt line on LIS3DSH
	GPIO_InitTypeDef GPIOE_Params; // Declares the structure handle for the parameters of GPIOE
	GPIO_InitTypeDef GPIOA_Params; // Declares the structure handle for the parameters of GPIOA
	GPIO_InitTypeDef GPIOE_Params_I; // Declares the structure handle for the parameters of the interrupt pin on GPIOE
	
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enables the clock for SPI1

	SPI_Params_adr->Instance = SPI1; // Selects which SPI interface to use
	SPI_Params_adr->Init.Mode = SPI_MODE_MASTER; // Sets the STM32F407 to act as the master
	SPI_Params_adr->Init.NSS = SPI_NSS_SOFT; // Sets the slave to be controlled by software
	SPI_Params_adr->Init.Direction = SPI_DIRECTION_2LINES; // Sets the SPI for communication in both directions
	SPI_Params_adr->Init.DataSize = SPI_DATASIZE_8BIT; // Sets the data packet size to 8-bit
	SPI_Params_adr->Init.CLKPolarity = SPI_POLARITY_HIGH; // Sets the idle state of the clock line to high
	SPI_Params_adr->Init.CLKPhase = SPI_PHASE_2EDGE; // Sets the data line to change on every second change of the clock line
	SPI_Params_adr->Init.FirstBit = SPI_FIRSTBIT_MSB; // Sets the transmission to MSB first
	
	SPI_Params_adr->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; // Sets the clock prescaler to divide the main APB2 clock (previously set to 84MHz with internal oscillator) by 32 to give a SPI clock of 2.625MHz
	
	HAL_SPI_Init(SPI_Params_adr); // Configures the SPI using the specified parameters
	
	// Code to initialise pins 5-7 of GPIOA
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; //Enable the clock for GPIOA
	GPIOA_Params.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7; // Selects pins 5,6 and 7
	GPIOA_Params.Alternate = GPIO_AF5_SPI1; //Selects alternate function 5 which corresponds to SPI1
	GPIOA_Params.Mode = GPIO_MODE_AF_PP; //Selects alternate function push-pull mode
	GPIOA_Params.Speed = GPIO_SPEED_FAST; //Selects fast speed
	GPIOA_Params.Pull = GPIO_NOPULL; //Selects no pull-up or pull-down activation
	HAL_GPIO_Init(GPIOA, &GPIOA_Params); // Sets GPIOA into the modes specified in GPIOA_Params
	
	
	// Code to initialise pin 3 of GPIOE
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // Enable the clock for GPIOE
	GPIOE_Params.Pin = GPIO_PIN_3; // Selects pin 3 to be configured
	GPIOE_Params.Mode = GPIO_MODE_OUTPUT_PP; // Selects push-pull mode
	GPIOE_Params.Speed = GPIO_SPEED_FAST; // Selects fast speed Pin operation
	GPIOE_Params.Pull = GPIO_PULLUP; //Selects internal pull-up resistor activation
	HAL_GPIO_Init(GPIOE, &GPIOE_Params); // Sets GPIOE into the modes specified in GPIOE_Params

	//Code to initialise GPIOE pin 0 for the interrupt
	GPIOE_Params_I.Pin = GPIO_PIN_0; // Selects pin 0
	GPIOE_Params_I.Mode = GPIO_MODE_IT_RISING; // Sets interrupt to rising edge
	GPIOE_Params_I.Speed = GPIO_SPEED_FAST; // Sets fast speed Pin operation
	HAL_GPIO_Init(GPIOE, &GPIOE_Params_I); // Sets GPIOE into the modes specified in GPIOE_Params_I
	
	GPIOE->BSRR |= GPIO_PIN_3; //Sets the SPI enable pin to high level (idle State)
	__HAL_SPI_ENABLE(SPI_Params_adr); //Enable the SPI
	
	// Write a new value to control register 3 of the LIS3DSH to configure the interrupts
	send_data = 0x23; // Address for control register 3 on the LIS3DSH
	GPIOE->BSRR = GPIO_PIN_3<<16; // Set the SPI communication enable line low to initiate communication
	HAL_SPI_Transmit(SPI_Params_adr,&send_data,1,1000); // Send the address of the register to be read on the LIS3DSH
	send_data = 0xC8; // Enable DRDY connected to Int1, sets Int1 active to high, enables int1
	HAL_SPI_Transmit(SPI_Params_adr,&send_data,1,1000); // Send the new register value to the LIS3DSH through the SPI channel
	GPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the communication process
}

// Definition for the function to write data to a register of LIS3DSH
// reg_adr: register address (8 Bit) of register to write to
// wrt_data: data to write
// SPI_Params_adr: Pointer to SPI communication parameters
void Write_LIS3DSH(uint8_t reg_adr, uint8_t wrt_data, SPI_HandleTypeDef *SPI_Params_adr){
	GPIOE->BSRR |= GPIO_PIN_3<<16; // Set SPI communication enable line to low => start communication
	HAL_SPI_Transmit(SPI_Params_adr,&reg_adr,1,1000); // Send the address of the register to be read on the LIS3DSH
	HAL_SPI_Transmit(SPI_Params_adr,&wrt_data,1,1000); // Send the data to write to the LIS3DSH through the SPI channel
	GPIOE->BSRR |= GPIO_PIN_3; // Set the SPI communication enable line to high => end of communication
}

// Definition for the function to read data from a register of LIS3DSH
// returns values in register to read
// reg_adr: register address (8 Bit) of register to read from
// SPI_Params: Pointer to SPI communication parameters
uint8_t Read_LIS3DSH(uint8_t reg_adr, SPI_HandleTypeDef *SPI_Params_adr){
	uint8_t send_data = 0x00; // Empty address needed for HAL_SPI_Receive function
	reg_adr |= 0x80; // set MSB to indicate read
	GPIOE->BSRR |= GPIO_PIN_3<<16; // Set SPI communication enable line to low => start communication
	HAL_SPI_Transmit(SPI_Params_adr,&reg_adr,1,1000); // Send the address of the register to be read on the LIS3DSH
	HAL_SPI_Receive(SPI_Params_adr,&send_data,1,1000); // Get the data from the LIS3DSH through the SPI channel
	GPIOE->BSRR |= GPIO_PIN_3; // Set the SPI communication enable line to high => end of communication
	return *((*SPI_Params_adr).pRxBuffPtr); // Read received data from the buffer and return it
}

// Definition for the function to initialisze the CS43L22 (I2C, I2S and basic register setup)
// I2S_Params: Pointer to I2S communication parameters
void Init_CS43L22(I2S_HandleTypeDef *I2S_Params_adr){
	
	// variables declaration
	uint32_t ii; // iteration variable for-loop
	I2C_HandleTypeDef I2C_Params; // declare Parameter Handle
	HAL_StatusTypeDef I2C_State; // declare State handle
	GPIO_InitTypeDef GPIOA_Params; // Declares the structure handle for the parameters of I2S on GPIOA
	GPIO_InitTypeDef GPIOD_Params; // Declares the structure handle for the parameters of the reset pin on GPIOE
	GPIO_InitTypeDef GPIOB_Params; // Declares the structure handle for the parameters of I2C on GPIOB
	GPIO_InitTypeDef GPIOC_Params; // Declares the structure handle for the parameters of I2S on GPIOC
	uint8_t send_data; // declare send data variable for I2C
	uint8_t send_data_array[2]; // declare send data variable for I2C
	uint8_t received_data; // declare received data variable for I2C
	RCC_ClkInitTypeDef RCC_ClkInitStruct; // Declares the structure handle for the parameters of the System Clock configuration
	RCC_PeriphCLKInitTypeDef RCC_I2S_PLL_CLK; // // Declares the structure handle for the parameters of the I2S PLL configuration
	RCC_OscInitTypeDef RCC_OscInitStruct; // Declares the structure handle for the parameters of the System Clock configuration
	uint16_t I2S_Data[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Declares data to be send over I2S
	
	
	// Ensure Clocks beeing properly set
  __PWR_CLK_ENABLE(); // enable power controll clock
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1); // allow HAL to optimize power consumption
  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE; // choose HSE type
  RCC_OscInitStruct.HSEState = RCC_HSE_ON; // activate HSE
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON; // set Phase locked loop
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE; // set source for PLL to HSE
  RCC_OscInitStruct.PLL.PLLM = 8; // divider => 8
  RCC_OscInitStruct.PLL.PLLN = 336; // multiplicator => 336
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; // divider => 2
  RCC_OscInitStruct.PLL.PLLQ = 7; // divider => 7
  HAL_RCC_OscConfig(&RCC_OscInitStruct); // set Clock
  
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; // set PLL as System Clock (168Mhz)
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; // set divider for Sysclock to 1 (168Mhz)
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  // set divider for APB1 to 4 (42Mhz)
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  // set divider for APB2 to 2 (84Mhz)
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5); // set Clock
	
	// Configure GPIOD for reset pin on CS43L22
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //Enable the clock for GPIOD
	GPIOD_Params.Pin = GPIO_PIN_4; // Select pin 4
	GPIOD_Params.Mode = GPIO_MODE_OUTPUT_PP; //Selects normal output push-pull mode
	GPIOD_Params.Speed = GPIO_SPEED_FAST; //Selects fast speed
	GPIOD_Params.Pull = GPIO_PULLDOWN; //Selects pull-down activation
	HAL_GPIO_Init(GPIOD, &GPIOD_Params); // Sets GPIOD into the modes specified in GPIOE_Params

	// Configure GPIOB for SCL (PB6) and SDA (PB9) on I2C1
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; //Enable the clock for GPIOB
	GPIOB_Params.Pin = GPIO_PIN_6 | GPIO_PIN_9; // Selects pins 6 and 9
	GPIOB_Params.Alternate = GPIO_AF4_I2C1; //Selects alternate function (I2C1)
	GPIOB_Params.Mode = GPIO_MODE_AF_OD; //Selects alternate function open drain mode
	GPIOB_Params.Speed = GPIO_SPEED_FAST; //Selects fast speed
	GPIOB_Params.Pull = GPIO_NOPULL; //Selects no pull-up or pull-down activation
	HAL_GPIO_Init(GPIOB, &GPIOB_Params); // Sets GPIOB into the modes specified in GPIOA_Params
	
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN  ; // Enables the clock for I2C1
	RCC->APB1ENR |= RCC_APB1ENR_SPI3EN  ; // Enables the clock for SPI3 (I2S)
	
	// configure I2S Phase locked loop (including divider and multiplicator)
	RCC_I2S_PLL_CLK.PeriphClockSelection = RCC_PERIPHCLK_PLLI2S; 
  RCC_I2S_PLL_CLK.PLLI2S.PLLI2SN = 352;
  RCC_I2S_PLL_CLK.PLLI2S.PLLI2SR = 2;
  HAL_RCCEx_PeriphCLKConfig(&RCC_I2S_PLL_CLK);
	__HAL_RCC_PLLI2S_ENABLE(); // Enable I2S Phase locked loop
	
	// Configure GPIOC for 7(MCLK), 10(SCLK) and 12(SDIN) on I2S
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; //Enable the clock for GPIOC
	GPIOC_Params.Pin = GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_12; // Selects pins 7(MCLK), 10(SCLK) and 12(SDIN)
	GPIOC_Params.Alternate = GPIO_AF6_SPI3; //Selects alternate function (I2C1)
	GPIOC_Params.Mode = GPIO_MODE_AF_PP; //Selects alternate function open drain mode
	GPIOC_Params.Speed = GPIO_SPEED_FAST; //Selects fast speed
	GPIOC_Params.Pull = GPIO_NOPULL; //Selects no pull-up or pull-down activation
	HAL_GPIO_Init(GPIOC, &GPIOC_Params); // Sets GPIOB into the modes specified in GPIOA_Params
	
	// Configure GPIOA for 4(WS)on I2S
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; //Enable the clock for GPIOA
	GPIOA_Params.Pin = GPIO_PIN_4; // Selects pins 4(WS)
	GPIOA_Params.Alternate = GPIO_AF6_SPI3; //Selects alternate function (I2C1)
	GPIOC_Params.Mode = GPIO_MODE_AF_PP; //Selects alternate function open drain mode
	GPIOC_Params.Speed = GPIO_SPEED_FAST; //Selects fast speed
	GPIOC_Params.Pull = GPIO_NOPULL; //Selects no pull-up or pull-down activation
	HAL_GPIO_Init(GPIOA, &GPIOA_Params); // Sets GPIOA into the modes specified in GPIOA_Params
	
	// Configure I2S
	I2S_Params_adr->Instance = SPI3; // use I2S3
	I2S_Params_adr->Init.Mode = I2S_MODE_MASTER_TX; // Processor is Master
	I2S_Params_adr->Init.Standard = I2S_STANDARD_PHILIPS; // use Philips standard
	I2S_Params_adr->Init.DataFormat = I2S_DATAFORMAT_16B; // set data length to 16 Bit
	I2S_Params_adr->Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE; // enable MCLK Output
	I2S_Params_adr->Init.AudioFreq = I2S_AUDIOFREQ_8K; // sampling rate of Audio Data
	I2S_Params_adr->Init.CPOL = I2S_CPOL_LOW; // set Clock polarity
	I2S_Params_adr->Init.ClockSource = I2S_CLOCK_PLL; // use I2S PLL Clock as Source
	I2S_Params_adr->Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE; // no full duplex 
	HAL_I2S_Init(I2S_Params_adr); // Initialise with HAL function
	__HAL_I2S_ENABLE(I2S_Params_adr); // enable I2S
	
	// Configure I2C
	I2C_Params.Instance = I2C1;
	I2C_Params.Init.ClockSpeed = 100000; // Set clock speed to 50 kHz
	I2C_Params.Init.DutyCycle = I2C_DUTYCYCLE_2; // Set duty cycle
	I2C_Params.Init.OwnAddress1 = 0x33; // set own adress 1 to 0x33		
	I2C_Params.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT; // set to 7-Bit adresses
	I2C_Params.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED; // Disabel dual address mode
	I2C_Params.Init.OwnAddress2 = 0; // set own adress 2
	I2C_Params.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED; // General call disabled
	I2C_Params.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED; // No Stretch disabled
	__HAL_I2C_ENABLE(&I2C_Params); // Enable the I2C1
	I2C_State = HAL_I2C_Init(&I2C_Params); // configure I2C
	
	GPIOD->BSRR |= GPIO_PIN_4; //Sets the reset pin of CS43L22 high
	
	for(ii=0; ii<260000; ii++){} // wait for startup of CS43L22 
	
	send_data_array[0] = CODEC_MAP_BEEP_VOL_OFFTIME; // adress of register to write
	send_data_array[1] = 0x06; // value for register => +6dB and 1.23 s off time
	I2C_State = HAL_I2C_Master_Transmit(&I2C_Params, 0x94, send_data_array, 2, 1000); // write register
	
	send_data_array[0] = CODEC_MAP_BEEP_FREQ_ONTIME; // adress of register to write
	send_data_array[1] = 0x69; // value for register => beep at 888.89 Hz and 1.8s OnTime
	I2C_State = HAL_I2C_Master_Transmit(&I2C_Params, 0x94, send_data_array, 2, 1000); // write register
	
	send_data_array[0] = CODEC_MAP_BEEP_TONE_CFG; // adress of register to write
	send_data_array[1] = 0x80; // value for register => multiple beep
	I2C_State = HAL_I2C_Master_Transmit(&I2C_Params, 0x94, send_data_array, 2, 1000); // write register
	
	send_data_array[0] = CODEC_MAP_PWR_CTRL2; // adress of register to write
	send_data_array[1] = 0xA0; // value for register => set both headphone channels always on
	I2C_State = HAL_I2C_Master_Transmit(&I2C_Params, 0x94, send_data_array, 2, 1000); // write register
	
	
	// Init prozedure from Datasheet of CS43L22 page 32
	send_data_array[0] = 0x00; // adress of register
	send_data_array[1] = 0x99; // value for register
	I2C_State = HAL_I2C_Master_Transmit(&I2C_Params, 0x94, send_data_array, 2, 10000); // set register
	
	send_data_array[0] = 0x47; // adress of register
	send_data_array[1] = 0x80; // value for register
	I2C_State = HAL_I2C_Master_Transmit(&I2C_Params, 0x94, send_data_array, 2, 10000); // set register
	
	send_data = 0x32; // adress of register to read
	I2C_State = HAL_I2C_Master_Transmit(&I2C_Params, 0x94, &send_data, 1, 1000); // Write Address to read
	I2C_State = HAL_I2C_Master_Receive(&I2C_Params, 0x95, &received_data, 1, 1000); // read register
	
	send_data_array[0] = 0x32; // adress of register
	send_data_array[1] = received_data | 0x80; // value for register => set MSB
	I2C_State = HAL_I2C_Master_Transmit(&I2C_Params, 0x94, send_data_array, 2, 10000); // set register
	
	send_data = 0x32; // adress of register to read
	I2C_State = HAL_I2C_Master_Transmit(&I2C_Params, 0x94, &send_data, 1, 1000); // Write Address to read
	I2C_State = HAL_I2C_Master_Receive(&I2C_Params, 0x95, &received_data, 1, 1000); // read register
	
	send_data_array[0] = 0x32; // adress of register
	send_data_array[1] = received_data & (~0x80); // value for register => reset MSB
	I2C_State = HAL_I2C_Master_Transmit(&I2C_Params, 0x94, send_data_array, 2, 10000); // set register
	
	send_data_array[0] = 0x00; // adress of register
	send_data_array[1] = 0x00; // value for register
	I2C_State = HAL_I2C_Master_Transmit(&I2C_Params, 0x94, send_data_array, 2, 10000); // set register
	
	send_data_array[0] = CODEC_MAP_PWR_CTRL1; // adress of register to write
	send_data_array[1] = 0x9E; // value for register => power up
	I2C_State = HAL_I2C_Master_Transmit(&I2C_Params, 0x94, send_data_array, 2, 1000); // write register
	//end of initialization sequence
	
	HAL_I2S_Transmit(I2S_Params_adr, I2S_Data, 10, 1000); // make first connection to audio
}