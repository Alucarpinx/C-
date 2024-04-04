// Name: Hector Garcia   |   Lab Section: 6 \\

#include <stdint.h>
#include "msp.h"
#include "../inc/CortexM.h"
#include "../inc/PWM.h"
#include <stdbool.h>
#include "..\inc\Clock.h"

void PortInit(void){
    P1DIR &= ~0x02; // 0b0000 0010 sets S1 as input
    P1REN |= 0x02; //resistor
    P3DIR |= 0x80;// 0b10000000
    P3OUT = 0x00;
}

void TimerInit(void){
    TA1CTL &= ~0x0030;//stop the timer
    TA1CTL |= 0x02C0;//choose  SMCLK  0b0000 0010 1100 0000  clk divider 8
    TA1EX0 = 0x0004; // clock divider of 5
    TA1CCR0 = 26999; // period
    TA1R = 0;//clear the timer
    }

void Delay180(void){
    TA1CTL &= ~0x0030;//stop the timer
    TA1R = 0;//clear the timer
    TA1CTL  |= 0x0020;//start timer in continuous mode
  while(!(TA1CCTL0 & 0x0001)){}; //complete while loop, wait for the CCIFG to assert
    TA1CCTL0 &= ~0x0001;//clear the CCIFG
    TA1CTL &= ~0x0030;//stop the timer
}

void main(void){
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	Clock_Init48MHz();
	PortInit(); //  calls ports initialization function
	TimerInit(); //  Initiates timer

	while(1){

	    if((P1IN & BIT1) == 0){  // active low
	        Delay180(); // delays for 180ms
	        P3OUT ^= 0x80; // 0b1000 0000
	    } //END If statement
	    TA1CTL &= ~0x0030;//stop the timer
	} //END while
} //END main
