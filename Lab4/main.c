

#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include "..\inc\Clock.h"
#include "..\inc\CortexM.h"
#include "..\inc\Init_Ports.h"
#include "..\inc\Motor.h"
#include "..\inc\PWM.h"

void TimerInit(void){
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
void DelayTenMilliseconds(void){
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
BumpInt_Init();
EnableInterrupts();


extern uint8_t wasInterrupt;
extern bool is_Left;
extern bool is_Right;

//declare enumerated states, declare starting state, declare previous state, declare state timer
    enum motor_states {Move_Forward, Move_Backward, Turn_Right, Turn_Left, Sleep_} state, prevState;

//declare boolean to know if state has switched use lecture 3 for state machine reference

    state =  Move_Forward;                  //start state
    prevState = !Move_Forward;               //used to know when the state has changed
    uint16_t stateTimer = 0;       //used to stay in a state
    bool isNewState;   //true when the state has switched
   // uint16_t rightDuty = 14999;
    //uint16_t leftDuty = 14999;
    stateTimer = 0;
    //bool BANK;

   while(1){
             isNewState = (state != prevState);
             prevState = state;  //save state for next time

            switch (state) {
            //each case below should have entry housekeeping, state business and exit housekeeping
            //remember to reset the stateTimer each time you enter a new state
            //you must assign a new state when stateTimer reaches the correct value
            case Move_Forward:
                    MotorForward(14999, 14999);
                if(wasInterrupt == 1){
                    wasInterrupt = 0;
                   state = Move_Backward;
                }
                    break;

            case Move_Backward:
                            if(isNewState){
                                stateTimer = 0;
                                MotorBackward(14999, 14999);
                            }
//                            if (( stateTimer >= 110) && (is_Right)) {
//                               is_Right = 0;
//                               state = Turn_Left;
//                               }
//                                else if (( stateTimer >= 110 ) && (is_Left)) {
//                                   is_Left = 0;
//                                  state = Turn_Right;
//                                }
//                             break;
                            if ( stateTimer >= 110){
                                if(is_Right == 1){
                                    is_Right = 0;
                                    state = Turn_Left;
                                }else if(is_Left == 1){
                                    is_Left = 0;
                                    state = Turn_Right;
                                }
                            }else{
                               //state = Move_Forward;
                            }
                            break;
            case Turn_Right:
                          if(isNewState){
                              stateTimer = 0;
                              MotorRight(7599, 9099);
                          }

                          //stateTimer++;
                          if(stateTimer == 175){
                              P4IFG &= 0x00; // clear interrupt flags
                              wasInterrupt = 0;
                             state = Move_Forward;
                          }
                              break;

            case Turn_Left:
                if(isNewState){
                    stateTimer = 0;
                    MotorLeft(9099, 7599);
                }

                //stateTimer++;
                if(stateTimer == 175){
                    P4IFG &= 0x00; // clear interrupt flags
                    wasInterrupt = 0;
                   state = Move_Forward;
                }
                    break;
            default:
                  state = Move_Forward;
                  break;
            } //switch
DelayTenMilliseconds();
stateTimer++;
    P2OUT ^= 0b00000100; //Toggles Blue light
         } //while(1)
     } //main()


