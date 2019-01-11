;****************** main.s ***************
; Program initially written by: Yerraballi and Valvano
; Author: Kenneth Berry
; Date Created: 1/15/2018 
; Last Modified: 10/22/2018 
;
; Brief description of the program: Fall 2018 Lab1
; The objective of this system is to implement odd-bit counting system
; Hardware connections: 
;  Output is positive logic, 1 turns on the LED, 0 turns off the LED
;  Inputs are negative logic, meaning switch not pressed is 1, pressed is 0
;    PE0 is an input 
;    PE1 is an input 
;    PE2 is an input 
;    PE3 is the output
; Overall goal: 
;   Make the output 1 if there is an odd number of 1's at the inputs, 
;     otherwise make the output 0

; The specific operation of this system 
;   Initialize Port E to make PE0,PE1,PE2 inputs and PE3 an output
;   Over and over, read the inputs, calculate the result and set the output

; NOTE: Do not use any conditional branches in your solution. 
;       We want you to think of the solution in terms of logical and shift operations

GPIO_PORTE_LOCK_R  EQU 0x40024520
GPIO_PORTE_CR_R    EQU 0x40024524
GPIO_PORTE_AMSEL_R EQU 0x40024528
GPIO_PORTE_PCTL_R  EQU 0x4002452C
GPIO_PORTE_AFSEL_R EQU 0x40024420
GPIO_PORTE_DR8R_R  EQU 0x40024508
GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_DEN_R   EQU 0x4002451C
SYSCTL_RCGCGPIO_R  EQU 0x400FE608
SYSCTL_RCGC2_GPIOE EQU 0x10  ; port D Clock Gating Control

GPIO_PE2           EQU 0x40024010 ; LED output
GPIO_PE3           EQU 0x40024020 ; SW1 input
GPIO_PE4           EQU 0x40024040 ; SW2 input
GPIO_PE5           EQU 0x40024080 ; SW3 input

      THUMB
      AREA    DATA, ALIGN=2

      ALIGN
      AREA    |.text|, CODE, READONLY, ALIGN=2
      EXPORT  Start

GPIO_Init
      ; Activate clock for Port E
      LDR R1, =SYSCTL_RCGCGPIO_R      
      LDR R0, [R1]                   
      ORR R0, R0, #SYSCTL_RCGC2_GPIOE ; clock
      STR R0, [R1]
      ; allow time to finish activating
      NOP
      NOP
      NOP
      NOP
      
      ; Disable analog functionality
      LDR R1, =GPIO_PORTE_AMSEL_R    
      LDR R0, [R1]                   
      BIC R0, R0, #0xFF 
      STR R0, [R1]
	  ; Configure as GPIO
      LDR R1, =GPIO_PORTE_PCTL_R   
      LDR R0, [R1]                 
      MOV R2, #0xFFFF          
      BIC R0, R0, R2                
      STR R0, [R1]
 
      ; Set direction register
      LDR R1, =GPIO_PORTE_DIR_R     
      LDR R0, [R1]                 
      ORR R0, R0, #0x4       
      STR R0, [R1]
      
      ; Disable alternate functions
      LDR R1, =GPIO_PORTE_AFSEL_R  
      LDR R0, [R1]              
      BIC R0, R0, #0xFF 
      STR R0, [R1]
      
      ; Enable digital ports
      LDR R1, =GPIO_PORTE_DEN_R     
      LDR R0, [R1]                
      ORR R0, R0, #0x3C
      STR R0, [R1]  
      BX  LR	
 
Start 
      BL  GPIO_Init
      LDR R8,=GPIO_PE2  ; Load address for PE2 into R8
      LDR R9,=GPIO_PE3  ; Load address for PE3 into R9
      LDR R10,=GPIO_PE4 ; Load address for PE4 into R10
      LDR R11,=GPIO_PE5 ; Load address for PE5 into R11
      MOV R4,#1         ; Move 0001 into R4

loop
      LDR R0,[R9]       ; Load value from PE3 to R0
      MOV R0,R0,LSR #3  ; Right shift R0 by 3
      LDR R1,[R10]      ; Load value from PE4 to R1
      MOV R1,R1,LSR #4  ; Right shift R1 by 4
      LDR R2,[R11]      ; Load value from PE5 to R2
      MOV R2,R2,LSR #5  ; Right shift R2 by 5
      EOR R3,R0,R1      ; R0 XOR R1 -> R3
      EOR R3,R3,R2      ; R2 XOR R3 -> R3
      EOR R3,R3,R4      ; R3 XOR 1  -> R3
      MOV R3,R3,LSL #2  ; Left shift R3 by 2
      STR R3,[R8]       ; Store R3 in PE2 address
      B   loop

      ALIGN        ; make sure the end of this section is aligned
      END          ; end of file
          
