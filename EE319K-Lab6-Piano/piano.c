// Piano.c
// This software configures the off-board piano keys
// Lab 6 requires a minimum of 3 keys, but you could have more
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
#include "tm4c123gh6pm.h"

// **************Piano_Init*********************
// Initialize piano key inputs, called once to initialize the digital ports
// Input: none 
// Output: none
void Piano_Init(void){
	SYSCTL_RCGCGPIO_R &= ~0x10;		// enable port E clock
	SYSCTL_RCGCGPIO_R |= 0x10;
	__nop();
	__nop();
	GPIO_PORTE_DIR_R &= ~0x0F;		// set PE0-3 to input
	GPIO_PORTE_DEN_R &= ~0x0F;		// enable GPIO for PE0-3
	GPIO_PORTE_DEN_R |= 0x0F;
}

// **************Piano_In*********************
// Input from piano key inputs 
// Input: none 
// Output: 0 to 7 depending on keys
//   0x01 is just Key0, 0x02 is just Key1, 0x04 is just Key2
//   bit n is set if key n is pressed
uint32_t key_input = 0;

uint32_t Piano_In(void){
	key_input = GPIO_PORTE_DATA_R & 0x0F;    // mask PE3 input
	return key_input; // return sum of inputs
}
