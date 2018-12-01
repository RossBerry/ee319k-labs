// Lab6.c
// Runs on LM4F120 or TM4C123
// Uses SysTick interrupts to implement a 4-key digital piano
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
#include "sound.h"
#include "piano.h"
#include "TExaS.h"
#include "dac.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); 	// Disable interrupts
void EnableInterrupts(void);  	// Enable interrupts	

// notes: note period values are 1/(freq * 12.5E-9 * 32)
unsigned long C3 = 19083;	// 131Hz
unsigned long D3 = 17006;	// 147Hz
unsigned long E3 = 15151;	// 165Hz
unsigned long F3 = 14285;	// 175Hz
unsigned long G3 = 12755;	// 196Hz
unsigned long A3 = 11363;	// 220Hz
unsigned long B3 = 10121; // 247Hz
unsigned long C4 = 9541;	// 262Hz
unsigned long D4 = 8503;	// 294Hz
unsigned long E4 = 7575;	// 330Hz
unsigned long F4 = 7163;	// 349Hz
unsigned long G4 = 6377;	// 392Hz
unsigned long A4 = 5681;	// 440Hz
unsigned long B4 = 5060;  // 494Hz
unsigned long C5 = 4780;  // 523Hz

unsigned long input = 0;				// Port E input
unsigned long f_input = 0;			// Port F input
unsigned long note = 0;         // sine wave period 

void PortF_Init(void){
	SYSCTL_RCGCGPIO_R &= ~0x20;		// enable port F clock
	SYSCTL_RCGCGPIO_R |= 0x20;
	__nop();
	__nop();
	GPIO_PORTF_DIR_R |= 0x10;		// set PF4 to input
	GPIO_PORTF_DEN_R |= 0x10;
	GPIO_PORTF_PDR_R |= 0x10;
}

int main(void){      
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
  PortF_Init();
	Piano_Init();
  Sound_Init();
	DAC_Init();
	EnableInterrupts();
	
	// main loop
  while(1){
		f_input = GPIO_PORTF_DATA_R &= 0x10;
//		if (f_input == 0){
//		  Change_Instrument();
//			while(f_input == 0){
//			f_input = GPIO_PORTF_DATA_R &= 0x10;
//			}
//		}
		input = Piano_In();						// read input
		switch(input){								// change note depending on input
			case 1:
				note = C3;
				break;
			case 2:
				note = D3;
				break;
			case 3:
				note = E3;
				break;
			case 4:
				note = F3;
				break;
			case 5:
				note = G3;
				break;
			case 6:
				note = A3;
				break;
			case 7:
				note = B3;
				break;
			case 8:
				note = C4;
				break;
			case 9:
				note = D4;
				break;
			case 10:
				note = E4;
				break;
			case 11:
				note = F4;
				break;
			case 12:
				note = G4;
				break;
			case 13:
				note = A4;
				break;
			case 14:
				note = B4;
				break;
			case 15:
				note = C5;
				break;
			default:
				note = 0;
				break;
		}
		Sound_Play(note);  // play note
  }  
}
