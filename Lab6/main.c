#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include "..\inc\Clock.h"
#include "..\inc\CortexM.h"
//#include "..\inc\Init_Ports.h"
//#include "..\inc\Motor.h"
#include "..\inc\PWM.h"
#include "..\inc\Reflectance.h"
//#include "../inc/Motor.h"


#define RIGHT_MOT_DIR    0x20  //0b0010 0000  P5.5
#define RIGHT_MOT_SLEEP  0x40  //0b0100 0000  P3.6
#define RIGHT_MOT_PWM    0x40  //0b0100 0000  P2.6
#define LEFT_MOT_DIR     0x10  //0b0001 0000  P5.4
#define LEFT_MOT_SLEEP   0x80  //0b1000 0000  P3.7
#define LEFT_MOT_PWM     0x80  //0b1000 0000  P2.7


void Port2_Init(void){
//LAB3 additions
    P2DIR |= 0xC0;    //P2.6 and P2.7 as outputs
    P2SEL1 &= ~0xC0;
    P2SEL0 |= 0xC0;


    P2DIR |= 0x07; //-> 0111 for lights
    P2OUT = 0x00;  //-> 0
}

void Port3_Init(void){
    P3DIR |= 0b11000000;
    P3OUT &= ~0b11000000;
}

void Port5_Init(void){
    P5DIR |=  0b00110000;
    P5OUT &= ~0b00110000;
}
void Port7_Init(void){
     // P5DIR |= 0x08;
     // P9DIR |= 0x04;
      P7DIR &= ~0xFF;

  //    P5OUT &= ^0x08;
  //    P9OUT &= ^0x04;

      P5OUT &= ~0x08;
      P9OUT &= ~0x04;

}


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
//Reflectance_Read(uint32_t time) // read input from sensors
//Reflectance_Position(uint8_t data) // Gives positional value


///// -0-0000000000000000000000000000000000000000000000

void MotorForward(uint16_t leftDuty, uint16_t rightDuty ){
    // This function sets the motors to drive forward with a PWM signal fixed at 10ms
    // having a duty cycle input when the function is called for each motor independently

     P3OUT |=  RIGHT_MOT_SLEEP; //wake up right motor
     P3OUT |=  LEFT_MOT_SLEEP;  //wake up left motor

     P5OUT &= ~RIGHT_MOT_DIR;   //set right motor in forward direction
     P5OUT &= ~LEFT_MOT_DIR;    //set left motor in forward direction

     //No need to drive P2OUT since Timer will drive it directly
     //This is configured via the PxSEL bits

     TA0R = 0;                  //counter, start at zero once turned on
     TA0CCR3  = rightDuty;      //right side high time goes in Capture/compare unit 3
     TA0CCR4  = leftDuty;       //left side high time goes in Capture/compare unit 4

     TA0CTL  |= 0x0010;         // start counting by setting mode to UP
     return;
   }


///// -0-0000000000000000000000000000000000000000000000
void main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer
    DisableInterrupts();
    Clock_Init48MHz();
    //UART0_Init();
    TimerInit();
    Port2_Init();
    Port3_Init();
    Port5_Init();
    Port7_Init();
    //unit8_t Transfer = 0;
    //time = 100;
    extern uint8_t Data;

    enum tracking_state{Forward, Right, Left}prevState, state;
        state = Forward;
        prevState = state;
        bool isNewState;
        int8_t position = 0;

//        while(0){
//            position = Reflectance_Read(800);
//            Clock_Delay1ms(20);
//        }
        while(1){
                 position = Reflectance_Position(Reflectance_Read(800)); // 200 * 1us
                 Clock_Delay1ms(20);
                 isNewState = (state != prevState);
                 prevState = state;  //save state for next time

                switch (state) {

                case Forward:
//                    if(isNewState){
//                      MotorForward(7999, 7999);
//                    }
                    MotorForward(7999, 7999);
                    P2OUT ^= 0b00000100;
                    if(position > 48){
                       state = Right;
                    }
                    if(position < -48){
                       state = Left;
                    }
                        break;

                case Right:
//                              if(isNewState){
//                                  MotorForward(7999, 7999);
//                              }
                              MotorForward(7999, 7999);
                              P2OUT ^= 0b00000010;
                              if(position > 48){
                                  MotorForward(5000, 5400);
                              }
                              if(position > 142){
                                  MotorForward(5000, 6099);
                              }
                              if(position > 238){
                                  MotorForward(5000, 7099);
                              }
                              if(position > 334){
                                 MotorForward(5000, 8099);
                              }
                              if(position < 0)
                                state = Forward;
                                  break;

                case Left:
//                              if(isNewState){
//                                  MotorForward(7999, 7999);
//                              }
                              MotorForward(7999, 7999);
                              P2OUT ^= 0b00000001;
                              if(position < -48){
                                  MotorForward(7499, 5000);
                              }
                              if(position < -142){
                                  MotorForward(8099, 5000);
                              }
                              if(position < -238){
                                  MotorForward(9099, 5000);
                              }
                              if(position < -334){
                                 MotorForward(10099, 5000);
                              }
                              if(position > 0)
                                state = Forward;
                                  break;
                } //switch case
//            P2OUT ^= 0b00000100; //Toggles Blue light
             } //while(1)
         } //main()
