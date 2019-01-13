;****************** main.s ***************
; Program written by: ***Your Names**update this***
; Date Created: 2/4/2017
; Last Modified: 8/24/2018
; Brief description of the program
;   The LED toggles at 2 Hz and a varying duty-cycle
; Hardware connections (External: One button and one LED)
;  PE2 is Button input  (1 means pressed, 0 means not pressed)
;  PE3 is LED output (1 activates external LED on protoboard)
;  PF4 is builtin button SW1 on Launchpad (Internal) 
;        Negative Logic (0 means pressed, 1 means not pressed)
; Overall functionality of this system is to operate like this
;   1) Make PE3 an output and make PE2 and PF4 inputs.
;   2) The system starts with the the LED toggling at 2Hz,
;      which is 2 times per second with a duty-cycle of 30%.
;      Therefore, the LED is ON for 150ms and off for 350 ms.
;   3) When the button (PE1) is pressed-and-released increase
;      the duty cycle by 20% (modulo 100%). Therefore for each
;      press-and-release the duty cycle changes from 30% to 70% to 70%
;      to 90% to 10% to 30% so on
;   4) Implement a "breathing LED" when SW1 (PF4) on the Launchpad is pressed:
;      a) Be creative and play around with what "breathing" means.
;         An example of "breathing" is most computers power LED in sleep mode
;         (e.g., https://www.youtube.com/watch?v=ZT6siXyIjvQ).
;      b) When (PF4) is released while in breathing mode, resume blinking at 2Hz.
;         The duty cycle can either match the most recent duty-
;         cycle or reset to 30%.
;      TIP: debugging the breathing LED algorithm using the real board.
; PortE device registers
GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_AMSEL_R EQU 0x40024528
GPIO_PORTE_PCTL_R  EQU 0x4002452C
GPIO_PORTE_AFSEL_R EQU 0x40024420
GPIO_PORTE_DEN_R   EQU 0x4002451C
SYSCTL_RCGC2_GPIOE EQU 0x00000010  ; port E Clock Gating Control
; PortF device registers
GPIO_PORTF_DATA_R  EQU 0x400253FC
GPIO_PORTF_DIR_R   EQU 0x40025400
GPIO_PORTF_AFSEL_R EQU 0x40025420
GPIO_PORTF_PUR_R   EQU 0x40025510
GPIO_PORTF_DEN_R   EQU 0x4002551C
GPIO_PORTF_LOCK_R  EQU 0x40025520
GPIO_PORTF_CR_R    EQU 0x40025524
SYSCTL_RCGC2_GPIOF EQU 0x00000020  ; Port F Clock Gating Control
; Miscellaneous registers
GPIO_LOCK_KEY      EQU 0x4C4F434B  ; Unlocks the GPIO_CR register
SYSCTL_RCGCGPIO_R  EQU 0x400FE608

       IMPORT  TExaS_Init
       THUMB
       AREA    DATA, ALIGN=2

;global variables
PE2    			   EQU 0x40024010 ; positive-logic button input 
PE3           	   EQU 0x40024020 ; positive-logic LED output
PF4		   		   EQU 0x40025040 ; negative-logic button input (on launchboard)

       AREA    |.text|, CODE, READONLY, ALIGN=2
       THUMB
       EXPORT  Start

Start
 ; TExaS_Init sets bus clock at 80 MHz
      BL  TExaS_Init ; voltmeter, scope on PD3
 ; initialization
	  ; activate clock for PortE
	  LDR R1, =SYSCTL_RCGCGPIO_R      
	  LDR R0, [R1]                   
	  ORR R0, R0, #SYSCTL_RCGC2_GPIOE ; clock
	  STR R0, [R1]
	  ; allow time to finish activating (4 cycles)
	  NOP		 ; 1
	  NOP	     ; 2
	  NOP        ; 3
	  NOP        ; 4
	  ; activate clock for PortF
	  ORR R0, R0, #SYSCTL_RCGC2_GPIOF ; clock
	  STR R0, [R1]
	  ; allow time to finish activating (4 cycles)
	  NOP		 ; 1
	  NOP	     ; 2
	  NOP        ; 3
	  NOP        ; 4
	  ; enable PortE digital ports
      LDR R1, =GPIO_PORTE_DEN_R     
      LDR R0, [R1]                
      ORR R0, R0, #0xFF
      STR R0, [R1]
      ; enable PortF digital ports
      LDR R1, =GPIO_PORTF_DEN_R     
      LDR R0, [R1]                
      ORR R0, R0, #0xFF
      STR R0, [R1]
	  ; configure PORTE as GPIO
      LDR R1, =GPIO_PORTE_PCTL_R   
      LDR R0, [R1]                 
      MOV R2, #0xFFFF          
      BIC R0, R0, R2                
      STR R0, [R1]               
      ; set PortE direction register
      LDR R1, =GPIO_PORTE_DIR_R     
      LDR R0, [R1]                 
      ORR R0, R0, #0x8       
      STR R0, [R1]
      ; set PortF direction register
      LDR R1, =GPIO_PORTF_DIR_R     
      LDR R0, [R1]                 
      ORR R0, R0, #0x0       
      STR R0, [R1]  	  
      ; disable PortE alternate functions
      LDR R1, =GPIO_PORTE_AFSEL_R  
      LDR R0, [R1]              
      ORR R0, R0, #0x0 
      STR R0, [R1]
      ; disable PortF alternate functions
      LDR R1, =GPIO_PORTF_AFSEL_R  
      LDR R0, [R1]              
      ORR R0, R0, #0x0 
      STR R0, [R1] 	  
	  ; enable pull-up resister for PF4
      LDR R1, =GPIO_PORTF_PUR_R     
      LDR R0, [R1]                
      ORR R0, R0, #0x10
      STR R0, [R1]
	  ; TExaS voltmeter, scope runs on interrupts
	  CPSIE  I
	  ; load address for PE2 into R2
	  LDR R2,=PE2
	  ; load address for PE3 into R3
	  LDR R3,=PE3
	  ; load address for PF4 into R4
	  LDR R4,=PF4
	  ; move 0 into R8 to start program with button unpressed
	  MOV R8,#0
	  ; move 30 into R10 to store duty cycle percentage, initialy 2hz 30%
	  MOV R10,#300
	  MOV R11,#1100

loop_1 
; main engine
; check for PF4 button press
     LDR R12,[R4]
	 CMP R12,#0x10
	 BNE PF4_press
; check for PE2 button press
     LDR R12,[R2]
	 CMP R12,#0
	 BEQ loop_1_1
	 B   loop_1_2
loop_1_1
	 CMP R8,#4
	 BEQ PE2_release
loop_1_2
     ; save button state into R8
	 MOV R8, R12
	 ; set PE3 high
	 MOV R0,#8
	 STR R0,[R3]      
	 ; delay for 150ms
	 MOV R5,R10
	 BL delay
loop_2
	 ; clear PE3 low
	 MOV R0,#0
	 STR R0,[R3]
	 ; delay for 350ms
	 MOV R0,#1000
	 SUB R5,R0,R10
	 BL delay
	 B  loop_1

delay
; delay for 0.5ms
     MOV R6,#9999
delay_loop
     SUBS R6,R6,#1
     BNE delay_loop
	 SUBS R5,R5,#1
	 BNE delay
	 BX LR

PE2_release
; increase the duty cycle by 20% 
; (if duty cycle = 110, set back to 10)
	 ADD R10,R10,#200
	 CMP R10,R11
	 BEQ reset_duty_cycle
	 B   loop_1_2
	 
reset_duty_cycle
; reset duty cycle back to 10%
	 MOV R10,#100
	 B   loop_1_2
	 
PF4_press
; initiate LED breathing
    ; store value in R10 in R9
	MOV R9,R10
	
LED_breathing
    ; recheck if PF4 is pressed
	LDR R12,[R4]
	CMP R12,#0x10
	BEQ PF4_release
	; store 2 in R5
	MOV R10,#0
	B   increase_brightness_loop
	
increase_brightness_loop
    ; set PE3 high
	MOV R0,#8
	STR R0,[R3]
	; increment duty cycle
	ADD R10,R10,#1
	CMP R10,#20
	BEQ decrease_brightness_loop
	MOV R5,R10
    ; delay
	BL  delay
	; set PE3 low
	MOV R0,#0
	STR R0,[R3]
	; dealy
	MOV R0,#20
	SUB R5,R0,R10
	BL  delay
	B   increase_brightness_loop
	
decrease_brightness_loop
	; set PE3 high
	MOV R0,#8
	STR R0,[R3]
	; decrement duty cycle
	SUB R10,R10,#1
	CMP R10,#0
	BEQ LED_breathing
	MOV R5,R10
    ; delay
	BL  delay
	; set PE3 low
	MOV R0,#0
	STR R0,[R3]
	; delay
	MOV R0,#20
	SUB R5,R0,R10
	BL  delay
	B   decrease_brightness_loop

PF4_release
; return to normal function
	; return value from R7 into R5
    MOV R10,R9
	B   loop_1

    ALIGN      ; make sure the end of this section is aligned
    
	END        ; end of file
