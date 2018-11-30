// dac.c
// This software configures DAC output
// Lab 6 requires a minimum of 4 bits for the DAC, but you could have 5 or 6 bits
// Runs on LM4F120 or TM4C123
// Program written by: Kenneth Berry
// Date Created: 11/15/18 
// Last Modified: 11/28/18 
// Lab number: 6
// Hardware connections:
//     PE0-3 are connected to button inputs
//     PB0-3 output to DAC circuit
//     PF2 outputs to heartbeat LED

#include <stdint.h>
#include "tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
	SYSCTL_RCGCGPIO_R	&= ~0x02;		// enable port B clock
  SYSCTL_RCGCGPIO_R |= 0x02;
	__nop();
	__nop();
	GPIO_PORTB_DIR_R &= ~0x0F;		// set PB0-3 to output
	GPIO_PORTB_DIR_R |= 0x0F;
	GPIO_PORTB_AFSEL_R &= ~0x07;  // disable alt funct on PB2-0
	GPIO_PORTB_DEN_R &= ~0x0F;		// enable GPIO for PB0-3
	GPIO_PORTB_DEN_R |= 0x0F;
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Input=n is converted to n*3.3V/15
// Output: none
void DAC_Out(uint32_t data){
	GPIO_PORTB_DATA_R = data;			// output data to DAC on PORTB
}
