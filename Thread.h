// Thread.h

// Thread 1 'Blink_Bl_Thread': Blinks blue LED
extern int Init_Blink_Bl_Thread(void); // declaration of the function to initialise the main thread function.
void Blink_Bl_Thread(void const *argument); // declaration of the main thread function.

// Thread 2 'Blink_Or_Thread': Blinks orange LED
extern int Init_Blink_Or_Thread(void); // declaration of the function to initialise the main thread function.
void Blink_Or_Thread(void const *argument); // declaration of the main thread function.

// Thread 3 'Button_Thread': Tests for pressing of user buttton B1
extern int Init_Button_Thread(void); // declaration of the function to initialise the main thread function.
void Button_Thread(void const *argument); // declaration of the main thread function.

// Thread 4 'Tap_Thread': Detect tapping on the board
extern int Init_Tap_Thread(SPI_HandleTypeDef* SPI_Params_adr); // declaration of the function to initialise the main thread function.
void Tap_Thread(void const* argument); // declaration of the main thread function.

// Thread 5 'Sequence_Thread': Initilisation and crossing sequence control
extern int Init_Sequence_Thread(void); // declaration of the function to initialise the main thread function.
void Sequence_Thread(void const *argument); // declaration of the main thread function.

// Thread 6 'Beep_Thread': Emit beeping sound through onboard DAC audio driver
extern int Init_Beep_Thread(I2S_HandleTypeDef* I2S_Params_adr); // declaration of the function to initialise the main thread function.
void Beep_Thread(void const* argument); // declaration of the main thread function.
