// Sound.h
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

#ifndef SOUND_H
#define SOUND_H
#include <stdint.h>														 

// Header files contain the prototypes for public functions
// this file explains what the module does

// **************Sound_Init*********************
// Initialize digital outputs and SysTick timer
// Called once, with sound/interrupts initially off
// Input: none
// Output: none
void Sound_Init(void);

void ChangeInstrument(void);

// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Play called again
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//        if period equals zero, disable sound output
// Output: none
void Sound_Play(uint32_t period);

void SysTick_Handler(void);

#endif

