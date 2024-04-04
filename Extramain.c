#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <Clock.h>
#include <CortexM.h>
//#include "..\inc\Clock.h"
//#include "..\inc\CortexM.h"

/** Output waveform: 20 ms period, 10% duty cycle
 *  The main program does the following:
 *  If a TimerA3 reaches CCRO interrupt is detected, Toggles output
 *  If TimerA3 CCR2 interrupt is detected, Toggles output
 **/
volatile bool INTCCR0 = 0;
volatile bool INTCCR2 = 0;

void Interrupt_Init (void) {
    P2DIR |= 0x04; // 0b00000000
    // CCRO Interrupt
    TA3CTL = 0x290; // SMCLK/ DIV 4/ UpMODE
    TA3CCR0 = 59999; // 20 ms
    TA3CCTL0 |= BIT4;
    NVIC -> ISER[0] |= 0x0004000; //INTISR[14]
    //CCR2 Interrupt
    //TA3CTL |= BIT1; <- for TAIFG
    //CCR2 is CCIFG flag
    NVIC -> ISER[0] |= 0x0008000; //INTISR[15]
    TA3CCTL0 &= ~BIT0; // clear CCIFG flags
    TA3CCR2 = 5999; // 10% duty

    TA3R = 0; // Timer set to zero
    EnableInterrupts();
    return;
}

void TA3_0_IRQHandler(void) {
    //only enters when CCRO interrupt
   INTCCR0 = 1; //Interrupt occurred
   TA3CCTL0 &= ~BIT0; //clear flag
}

void TA3_N_IRQHandler(void) {
    uint16_t status = TA3IV; // reading clears flag
    if (status == 0x0004) {
        INTCCR2 = 1;
        return;
    }
}

void main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    Clock_Init48MHz();
    Interrupt_Init();
    P2OUT = 0b00000100; // Set high at first
    while(1){
      if (INTCCR2){
       P2OUT = 0x00; //  set low establishing a high time of 2 ms; P2OUT ^= 0x04; or P2OUT &= ~0x04;
       INTCCR2 = 0;
      }
      if (INTCCR0){ // after 20 ms sets back to high
       P2OUT = 0x04; //  P2OUT |= 0x04;
       INTCCR0 = 0;
      }
  }
}

