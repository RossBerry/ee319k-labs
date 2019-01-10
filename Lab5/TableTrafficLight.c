// TableTrafficLight.c
// Runs on LM4F120/TM4C123
// Use a table implementation of a Moore finite state machine to operate
// a traffic light.
// Kenneth Berry
// November 13, 2018

// walk button sensor connected to PE2 (1=button pressed)
// north facing car detector connected to PE1 (1=car present)
// east facing car detector connected to PE0 (1=car present)
// east facing red light connected to PB5
// east facing yellow light connected to PB4
// east facing green light connected to PB3
// north facing red light connected to PB2
// north facing yellow light connected to PB1
// north facing green light connected to PB0
// walk blue light connected to PF3
// walk green light connected to PF2
// walk red light connected to PF1

// input values: 
// 0x0=000= no sensors
// 0x1=001= west sensor
// 0x2=010= south sensor
// 0x3=011= west & south sensors
// 0x4=100= walk sensor
// 0x5=101= walk & west sensors
// 0x6=110= walk & south sensors
// 0x7=111= walk & west & south sensors

// output values for port B:
// 0x0C=001100;
// 0x14=010100;
// 0x21=100001;
// 0x22=100010;
// 0x24=100100;

// output values for port F:
// 0x1=001;
// 0x7=111;


#include <stdint.h>
#include "PLL.h"
#include "SysTick.h"

// portB registers
#define GPIO_PORTB_DATA_R       (*((volatile uint32_t *)0x400053FC))
#define GPIO_PORTB_DIR_R        (*((volatile uint32_t *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile uint32_t *)0x40005420))
#define GPIO_PORTB_DEN_R        (*((volatile uint32_t *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile uint32_t *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile uint32_t *)0x4000552C))
// portE registers
#define GPIO_PORTE_DATA_R       (*((volatile uint32_t *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile uint32_t *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile uint32_t *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile uint32_t *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile uint32_t *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile uint32_t *)0x4002452C))
// portF registers
#define GPIO_PORTF_DATA_R       (*((volatile uint32_t *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile uint32_t *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile uint32_t *)0x40025420))
#define GPIO_PORTF_DEN_R        (*((volatile uint32_t *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile uint32_t *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile uint32_t *)0x4002552C))
// clock registers
#define SYSCTL_RCGCGPIO_R       (*((volatile uint32_t *)0x400FE608))
#define SYSCTL_PRGPIO_R         (*((volatile uint32_t *)0x400FEA08))
// states
#define goN   0
#define waitN 1
#define waitSafetyN 2
#define walkN 3
#define walkWarningN 4
#define goE 5
#define waitE 6
#define waitSafetyE 7
#define walkE 8
#define walkWarningE 9

// state Structure
struct State {
  uint32_t OutB;      // 6-bit output for port B
	uint32_t OutF;     // 2-bit output for port F
  uint32_t Time;     // 10 ms
  uint8_t Next[8];}; // depends on 2-bit input

// FSM of states
typedef const struct State STyp;
STyp FSM[10]={
  // goN
 {0x0C,0x2,400,{waitN,waitN,goN,waitN,waitN,waitN,waitN,waitN}},
 // waitN
 {0x14,0x2,160,{waitSafetyN,waitSafetyN,waitSafetyE,waitSafetyN,walkN,walkN,walkN,walkN}},
  // waitSafetyN
 {0x24,0x2,80,{goE,goE,goN,goE,walkN,walkN,walkN,walkN}},
  // walkN
 {0x24,0xE,400,{walkWarningN,walkWarningN,walkWarningE,walkWarningN,walkN,walkN,walkN,walkN}},
   // walkWarningN
 {0x24,0xE,200,{goE,goE,goN,goE,walkN,walkN,walkN,walkN}},
 // goE
 {0x21,0x2,400,{waitE,goE,waitE,waitE,waitE,waitE,waitE,waitE}},
 // waitE
 {0x22,0x2,160,{waitSafetyE,waitSafetyN,waitSafetyE,waitSafetyE,walkE,walkE,walkE,walkE}},
   // waitSafetyE
 {0x24,0x2,80,{goN,goE,goN,goN,walkE,walkE,walkE,walkE}},
 // walkE
 {0x24,0xE,400,{walkWarningE,walkWarningN,walkWarningE,walkWarningE,walkE,walkE,walkE,walkE}},
 // walkWarningE
 {0x24,0xE,200,{goN,goE,goN,goN,walkN,walkN,walkN,walkN}}
};

// initialize port B
void portB_Init(void){
	GPIO_PORTB_DIR_R |= 0x3F;         // make PB5-0 out
  GPIO_PORTB_AFSEL_R &= ~0x3F;      // disable alt funct on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;         // enable digital I/O on PB5-0
                                    // configure PB5-0 as GPIO
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFF000000)+0x00000000;
  GPIO_PORTB_AMSEL_R &= ~0x3F;      // disable analog functionality on PB5-0
}

// initialize port E
void portE_Init(void){
	GPIO_PORTE_DIR_R &= ~0x7;        // make PE2-0 in
  GPIO_PORTE_AFSEL_R &= ~0x7;      // disable alt funct on PE2-0
  GPIO_PORTE_DEN_R |= 0x7;         // enable digital I/O on PE2-0
                                    // configure PE1-0 as GPIO
  GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFFFF000)+0x00000000;
  GPIO_PORTE_AMSEL_R &= ~0x7;      // disable analog functionality on PE2-0
}

// initialize port F
void portF_Init(void){
	GPIO_PORTF_DIR_R |= 0xE;        // make PE3-1 out
  GPIO_PORTF_AFSEL_R &= ~0xE;      // disable alt funct on PE3-1
  GPIO_PORTF_DEN_R |= 0xE;         // enable digital I/O on PE3-1
                                    // configure PE3-1 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFFF000F)+0x00000000;
  GPIO_PORTF_AMSEL_R &= ~0xE;      // disable analog functionality on PE3-1
}

// flash walk warning signal - flash red 4 times
void flash_warning(uint32_t time){
	time /= 8;
	int8_t count = 5;
	while(count--){
		GPIO_PORTF_DATA_R = 0x0;
		SysTick_Wait10ms(time);
		GPIO_PORTF_DATA_R = 0x2;
		SysTick_Wait10ms(time);
	}
}

// main function
int main(void){
	// initialize 80 MHz system clock
  PLL_Init(Bus80MHz);
	// initialize SysTick timer
  SysTick_Init();                   
	// activate clock for Port B,E,F
  SYSCTL_RCGCGPIO_R |= 0x32;
  // allow time for clock to stabilize
  while((SYSCTL_PRGPIO_R&0x32) == 0){};
	// initialize ports
	portB_Init();
	portE_Init();
	portF_Init();
	// initial state: Green north; Red east
  uint8_t n = goN;
  // Input from Port E Data register		
	uint32_t Input = 0xFF;
	// main loop
  while(1){
		// set lights to current state's Out value
    GPIO_PORTB_DATA_R = FSM[n].OutB;
	  // check if state is walkWarningN or walkWarningE
		if(n==4 || n==9){
			// flash walk warning signal
			flash_warning(FSM[n].Time);
		}
		else {
			// set lights to current state's Out value
			GPIO_PORTF_DATA_R = FSM[n].OutF;
			// wait 10 ms * current state's Time value
			SysTick_Wait10ms(FSM[n].Time);
    }
		// get new input from car detectors
    Input = GPIO_PORTE_DATA_R;
		// transition to next state
    n = FSM[n].Next[Input];        
  }
}
