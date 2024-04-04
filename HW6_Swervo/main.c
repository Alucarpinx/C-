#include "msp.h"
#include <msp432.h>
#include <stdint.h>
#include <stdbool.h>
#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/PWM.h"

void portsInit(void){
    // PushButtons = P1.1 (s1) , P1.4 (s2)
    // Lights = P2.0(red), P2.1(green) and P2.2(blue)
        P1 -> DIR &= ~0x12; // 0b 0001 0010
        P1 -> REN |= 0x12;
        P1 -> OUT |= 0x12;

        P2 -> DIR |= 0x07; // 0b 0000 0111
        P2 -> OUT = 0x00;
}

void timerInit(void){
    // P6.6 TA2.3 CCR3
    P6DIR |= 0x40; // 0b 0100 0000
    P6SEL1 &= ~0x40;
    P6SEL0 |= 0x40;

    TA2CTL |= 0x0020; // sm clk
    TA2CTL |= 0x0080; // clk divider of 4
    TA2CCTL3 |= 0X00E0; // reset/set OUTMOD 7
    TA2CCR0 = 59999; // period

}

void delayFunc(void){
    //***********Now initialize TimerAx for the delay function************//
    // delays 10ms
        TA1CTL &= ~0x0030;//stop the timer
        TA1CTL |= 0x0250;//choose clock divider of 2
        TA1CTL &= ~0x0100; // smclk
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
	portsInit();
	timerInit();
	while(1){
	    TA2CCR3 = 5999;  // pulse width or high time
	    TA2CTL |= 0x0010; // Up mode starts PWM

	    if((P1IN & BIT1) == 0){  // button s1
	        TA2CCR3 += 5999;
	        while((P1IN & BIT1) == 0); // wait for pin release
	    }
	    if((P1IN & BIT4) == 0){  // button s2
	        TA2CCR3 -= 5999;
	        while((P1IN & BIT4) == 0); // wait for pin
	    }
	delayFunc(); //delays 10ms
	delayFunc(); // delays another 10ms
	P2OUT ^= 0b00000001; //toggle red light every 20ms
	}
}
