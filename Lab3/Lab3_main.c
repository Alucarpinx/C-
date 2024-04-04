// Motor.c
// Runs on MSP432
// Provides functions that control robot motion
// using built in TimerA module
// Prof. Dickens
// 1/26/2023

/* These functions will be used in all of the labs for CPET253
// Left motor direction connected to P5.4
// Left motor PWM connected to P2.7/TA0CCP4
// Left motor enable connected to P3.7
// Right motor direction connected to P5.5
// Right motor PWM connected to P2.6/TA0CCP3
// Right motor enable connected to P3.6 */

#include <stdint.h>
#include "msp.h"
#include "../inc/CortexM.h"
#include "../inc/PWM.h"

#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include "..\inc\Clock.h"
#include "..\inc\CortexM.h"
#include "..\inc\Init_Ports.h"
#include "../inc/Motor.h"

#define RIGHT_MOT_DIR    0x20  //0b0010 0000  P5.5
#define RIGHT_MOT_SLEEP  0x40  //0b0100 0000  P3.6
#define RIGHT_MOT_PWM    0x40  //0b0100 0000  P2.6
#define LEFT_MOT_DIR     0x10  //0b0001 0000  P5.4
#define LEFT_MOT_SLEEP   0x80  //0b1000 0000  P3.7
#define LEFT_MOT_PWM     0x80  //0b1000 0000  P2.7


void TimerInit(void)

{
//**********First initialize TimerA0 for PWM
//Since the motors are connected to P2.6 and P2.7, use TimerA0, compare blocks 3 & 4****
    TA0CTL &= ~0x0030;//stop the timer
//choose SMCLK for the clock source
    TA0CTL |= 0x0250;//choose clock divider of 2
    TA0CCTL3 |= 0x00E0;//Outmode 7: reset/set
    TA0CCTL4 |= 0x00E0;//Outmode 7: reset/set
    TA0CCR0 = 59999;


//***********Now initialize TimerAx for the delay function************//
    TA1CTL &= ~0x0030;//stop the timer
    //SMCLK
    TA1CTL |= 0x0250;//choose clock divider of 2
    TA1CTL &= ~0x0100;
//    TA2CCTL1 |= BIT7&BIT6&BIT5;
    TA1CTL  |= 0x0020;//start timer in continuous mode

//choose clock divider of 2
}
void DelayTenMilliseconds(void)

{
//***********Start the delay and wait************//
   // TA2CCR0 = 59999;
    //TA2CTL |= 0x0020;

    TA1CTL &= ~0x0030;//stop the timer
    TA1R = 0;//clear the timer
    TA1CTL  |= 0x0020;//start timer in continuous mode
while(!(TA1CCTL0 & 0x0001)){}; //complete while loop, wait for the CCIFG to assert
    TA1CCTL0 &= ~0x0001;//clear the CCIFG
    TA1CTL &= ~0x0030;//stop the timer
}



void main(void)
{
WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer
Clock_Init48MHz(); // makes bus clock 48 MHz
TimerInit();
// call port initialization functions
    Port2_Init();
    Port3_Init();
    Port5_Init();
//declare enumerated states, declare starting state, declare previous state, declare state timer
    enum motor_states {Move_Forward, Move_Backward, Turn_Right, Turn_Left, Sleep_} state, prevState;

//declare boolean to know if state has switched use lecture 3 for state machine reference

    state =  Move_Forward;                  //start state
    prevState = !Move_Forward;               //used to know when the state has changed
    uint16_t stateTimer = 0;       //used to stay in a state
    bool isNewState;   //true when the state has switched
    uint16_t rightDuty = 14999;
    uint16_t leftDuty = 14999;
    stateTimer = 0;
    //bool BANK;

   while(1)
         {
             isNewState = (state != prevState);
             prevState = state;  //save state for next time

            switch (state) {
            //each case below should have entry housekeeping, state business and exit housekeeping
            //remember to reset the stateTimer each time you enter a new state
            //you must assign a new state when stateTimer reaches the correct value
            case Move_Forward:

                if(isNewState){
                    stateTimer  = 0;
                    //MotorStop();
                    //MotorForward(leftDuty, rightDuty);
                }
                MotorForward(14999, 14999);
                stateTimer++;
                if(stateTimer == 95){
                   state = Turn_Left;
                }
                    break;

            case Turn_Left:
                if(isNewState){
                    stateTimer = 0;
                }
                MotorLeft(9099, 7599);
                stateTimer++;
                if(stateTimer == 65){
                   state = Move_Forward;
                }
                    break;
            default:
                  state = Move_Forward;
                  break;
            } //switch
DelayTenMilliseconds();
    P2OUT ^= 0b00000100; //Toggles Blue light
         } //while(1)
     } //main()
