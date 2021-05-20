// my_header.h

#include "stm32f4xx.h"


// defines of CS43L22 (Datasheet page 35)
#define CODEC_MAP_CHIP_ID 0x01
#define CODEC_MAP_PWR_CTRL1 0x02
#define CODEC_MAP_PWR_CTRL2 0x04
#define CODEC_MAP_CLK_CTRL  0x05
#define CODEC_MAP_IF_CTRL1  0x06
#define CODEC_MAP_IF_CTRL2  0x07
#define CODEC_MAP_PASSTHROUGH_A_SELECT 0x08
#define CODEC_MAP_PASSTHROUGH_B_SELECT 0x09
#define CODEC_MAP_ANALOG_SET 0x0A
#define CODEC_MAP_PASSTHROUGH_GANG_CTRL 0x0C
#define CODEC_MAP_PLAYBACK_CTRL1 0x0D
#define CODEC_MAP_MISC_CTRL 0x0E
#define CODEC_MAP_PLAYBACK_CTRL2 0x0F
#define CODEC_MAP_PASSTHROUGH_A_VOL 0x14
#define CODEC_MAP_PASSTHROUGH_B_VOL 0x15
#define CODEC_MAP_PCMA_VOL 0x1A
#define CODEC_MAP_PCMB_VOL 0x1B
#define CODEC_MAP_BEEP_FREQ_ONTIME 0x1C
#define CODEC_MAP_BEEP_VOL_OFFTIME 0x1D
#define CODEC_MAP_BEEP_TONE_CFG 0x1E
#define CODEC_MAP_TONE_CTRL 0x1F
#define CODEC_MAP_MASTER_A_VOL 0x20
#define CODEC_MAP_MASTER_B_VOL 0x21
#define CODEC_MAP_HP_A_VOL 0x22
#define CODEC_MAP_HP_B_VOL 0x23
#define CODEC_MAP_SPEAK_A_VOL 0x24
#define CODEC_MAP_SPEAK_B_VOL 0x25
#define CODEC_MAP_CH_MIX_SWAP 0x26
#define CODEC_MAP_LIMIT_CTRL1 0x27
#define CODEC_MAP_LIMIT_CTRL2 0x28
#define CODEC_MAP_LIMIT_ATTACK 0x29
#define CODEC_MAP_OVFL_CLK_STATUS 0x2E
#define CODEC_MAP_BATT_COMP 0x2F
#define CODEC_MAP_VP_BATT_LEVEL 0x30
#define CODEC_MAP_SPEAK_STATUS 0x31
#define CODEC_MAP_CHARGE_PUMP_FREQ 0x34


// Declaration for the function to initialise all LEDs
void LED_Init(void);
	

// Declaration for the function to set state of green LED
// state = 1 to turn LED on; state = 0 to turn LED off
void Set_State_Gn_LED(uint8_t state);


// Declaration for the function to set state of red LED
// state = 1 to turn LED on; state = 0 to turn LED off
void Set_State_Rd_LED(uint8_t state);


// Declaration for the function to set state of orange LED
// state = 1 to turn LED on; state = 0 to turn LED off
void Set_State_Or_LED(uint8_t state);


// Declaration for the function to set state of blue LED
// state = 1 to turn LED on; state = 0 to turn LED off
void Set_State_Bl_LED(uint8_t state);


// Declaration for the function to initialize user Button B1
void B1_Init(void);


// Declaration for the function giving state of user button B1
// returns: 1 when pressed; 0 when not pressed
int Get_State_B1(void);


// Declaration for the function initialising timing via timer 2
// set_Time is time in multiples of 100µs after which timer flag is set
void Init_TIM2(uint32_t set_Time);


// Declaration for the function signaling state of timer 2
// returns 1 if time has elapsed
// returns 0 if time has not yet elapsed
uint8_t Get_State_TIM2(void);


// Declarationfor the function to enable timer 2 and reset overflow flag
void Enable_TIM2(void);


// Declarationfor for the function to intialise communication with LIS3DSH over SPI including interupt settings
// SPI_Params_adr: Pointer to Structure of SPI Parameters
void Init_LIS3DSH(SPI_HandleTypeDef* SPI_Params_adr);


// Declaration for the function to write data to a register of LIS3DSH
// reg_adr: register address (8 Bit) of register to write to
// wrt_data: data to write
// SPI_Params_adr: Pointer to SPI communication parameters
void Write_LIS3DSH(uint8_t reg_adr, uint8_t wrt_data, SPI_HandleTypeDef *SPI_Params_adr);


// Declaration for the function to read data from a register of LIS3DSH
// returns values in register to read
// reg_adr: register address (8 Bit) of register to read from
// SPI_Params: Pointer to SPI communication parameters
uint8_t Read_LIS3DSH(uint8_t reg_adr, SPI_HandleTypeDef *SPI_Params_adr);


// Declaration for the function to initialisze the CS43L22 (I2C, I2S and basic register setup)
// I2S_Params: Pointer to I2S communication parameters
void Init_CS43L22(I2S_HandleTypeDef *I2S_Params_adr);
