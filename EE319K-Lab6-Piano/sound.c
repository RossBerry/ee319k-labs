// Sound.c
// This module contains the SysTick ISR that plays sound
// Runs on LM4F120 or TM4C123
// Program written by: Kenneth Berry
// Date Created: 11/15/18 
// Last Modified: 11/28/18 
// Lab number: 6
// Hardware connections:
//     PE0-3 are connected to button inputs
//     PB0-3 output to DAC circuit
//     PF2 outputs to heartbeat LED

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "sound.h"
#include "dac.h"
#include "tm4c123gh6pm.h"
																	
uint8_t table_index = 0;	// current index read from wave form table
uint8_t instrument_index = 0; // current index of instrument array

// sound wave forms
// Sine Wave
uint8_t SineWave[32] ={8,9,10,12,13,14,14,15,
											 15,15,14,14,13,12,10,9,
											 8,6,5,3,2,1,1,0,
                       0,0,1,1,2,3,5,6};
// Guitar
uint8_t Guitar[32] ={5,5,4,1,1,3,8,11,
										 11,9,4,2,5,11,15,13,
										 9,7,5,5,6,8,8,7,
                     4,3,3,3,3,4,5,5};
// Bassoon
uint8_t Bassoon[32] ={8, 8,8,8,7,7,9,15,
											10,1,1,4,8,11,10,6,
											3,2,6,10,8,5,5,5,
                      6,7,7,9,8,8,8,7};
// Trumpet
uint8_t Trumpet[32] ={10,11,11,12,10,8,3,1,
											8,15,15,11,10,10,11,10,
											10,10,10,10,10,10,10,11,
                      11,11,11,11,11,10,10,10};
// array of instruments
uint8_t *Instruments[4]= {SineWave,Guitar,Bassoon,Trumpet};

// **************Sound_Init*********************
// Initialize digital outputs and SysTick timer
// Called once, with sound/interrupts initially off
// Input: none
// Output: none

void Sound_Init(void){
	// initialize SysTick
	NVIC_ST_CTRL_R = 0x00;				// clear ENABLE, stop counter
	NVIC_ST_RELOAD_R = 0x00;      // specify reload value
	NVIC_ST_CURRENT_R = 0;				// clear CURRENT
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x20000000; // set priority to 1
	NVIC_ST_CTRL_R = 0x07;				// enable ENABLE, CLK_SRC (INTERRUPT ENABLED)
}

// **************Change Instrument*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Play called again
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//           Units of period to be determined by YOU
//           Maximum period to be determined by YOU
//           Minimum period to be determined by YOU
//         if period equals zero, disable sound output
// Output: none
void Change_Instrument(){
	instrument_index++;
	if (instrument_index > 4){
	  instrument_index = 0;
	}
}

// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Play called again
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//           Units of period to be determined by YOU
//           Maximum period to be determined by YOU
//           Minimum period to be determined by YOU
//         if period equals zero, disable sound output
// Output: none

void Sound_Play(uint32_t period){
	NVIC_ST_RELOAD_R = period;		// specify reload value
}

void SysTick_Handler(void){
	table_index = table_index & 0x1F;		  // sets index back to 0 if it exceeds index 31 
	DAC_Out(Instruments[instrument_index][table_index]);   // outputs index value
	table_index++;									  // increment index
}	
