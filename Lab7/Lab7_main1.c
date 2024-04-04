#include "msp.h"
#include <msp432.h>
#include <stdint.h>
#include <stdbool.h>
#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/motor.h"
#include "../inc/Init_Ports.h"

#define TRIGGER 0x04
#define ECHO 0x08


#define microsecondsToClockCycles(a) ( (a) * 1.5 )       //assume 12Mhz clock divided by 8
#define clockCyclesToMicroseconds(a) ( (a) / 1.5 )       // 1.5 clock cycles = 1us

void Servo(uint16_t angle);
uint32_t pulseIn (void);

void ServoInit(void){
    Servo(11999);     //call Servo() function to center servo
    Clock_Delay1us(500);     //delay here to give servo time to move - can use built in timer function
    TA3CTL &= ~0x0030;//stop the timer
    return;
}
void Servo(uint16_t angle_count){

    TA3CCR0 = 59999;      //set period for 20ms
    TA3CCR2 = angle_count * (112);      //set high time for the input angle using angle_count, multiplied by WIdth for one degree change
    TA3CTL = 0x0010;      //set timer for up mode
    return;
}
uint16_t distanceInCm(void) {
    uint16_t distance;


    distance = ((pulseIn()) * 34)/2000;      //calculate distance using s=t * 0.034/2. t comes from pulseIn() function
    if(distance == 0){
        printf("Object too far");
    }else{
        printf("Distance = %d \n", distance);
    };      // if no echo (distance = 0), assume object is at farthest distance
    return distance;      //return the distance
}
uint32_t pulseIn (void){
    uint16_t width = 0;
    uint16_t time = 0;
    uint16_t maxcount = 56999;  //max count for 38 ms (timeout)
    TA2CTL |= 0x0020;           //set timer for continuous mode
    //P6.3 is echo

//    for (int i = 0; i< 8; i++){

    P6OUT |= 0x04;      //drive trigger pin high
    Clock_Delay1us(10);      //wait 10 us - can use built-in timer function
    P6OUT &= 0x04;      //drive trigger pin low
//}
    TA2R = 0;                   //reset the count
        while(!(P6IN & ECHO)){                    //wait for the pulse to finish (while Echo is low)
        if(TA2R > maxcount ){
            return 0;
        }
        }
    TA2R = 0;                   //reset the count
    while((P6IN & ECHO)){                    //wait for the pulse to finish (while Echo is high)
    if(TA2R > maxcount ){
        return 0;
    }
    }//if count is greater than maxcount return 0
    width = TA2R;               //read the count (width of the return pulse)
    TA2CTL &= ~0x0030;;               //stop the timer
    time = clockCyclesToMicroseconds(width);               // convert the reading to microseconds.
    return time;               //return the microsecond reading
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
//    ------------
    //TimerA3, CCR2 will be used for the servo PWM. Scale your clock accordingly to create
    //a 20ms period.
    TA3CCTL2 |= 0x00E0;
    //TA3CTL &= ~0x0030;
    TA3CTL |= 0x0200;
    TA3CTL |= 0x0080;
    TA3CCR0 = 59999;
    TA3CTL |= 0x0010;
    //TA3CCR2 = 1200; // change to set Servo angle
    //Timer A2 will be used for measuring the return pulse from the ultrasonic sensor. The
    //clock division for this timer needs to be 8.
    //TA2CTL = 0x02D0;// smclk  0000 0010 1101 0000
    //TA2CTL  |= 0x0200;
    //TA2CTL &= ~0x0010;
    //TA2CTL |= 0x00
    //DIV 8
    // UP tO CCR0 mode
    //TA2CTL &= ~0x0030;//stop the timer
    //TA2CTL |= 0x0270;//choose clock divider of 8
    //TA2CTL &= ~0x0100;


    TA2CTL |= 0x0200; // smclk
    TA2CTL |= 0x00C0; // div of 8
    TA2CTL |= 0x0010; // up mode


}

void main(void){
    uint16_t distance, right_wall, left_wall, LeftDistance, RightDistance;

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	Clock_Init48MHz();  // makes bus clock 48 MHz
	//call all the port initialization functions
	Port2_Init();
	Port3_Init();
	Port5_Init();
	Port6_Init();
	Port8_Init();
	TimerInit();
	ServoInit();

	//These are the states of the state machine
	enum motor_states {FORWARD, BACKWARD, RIGHT, LEFT, SWEEP_RIGHT, SWEEP_LEFT} state, prevState;

	state = FORWARD;          //start in FORWARD state
	prevState = !FORWARD;   //used to know when the state has changed
	uint16_t stateTimer = 0;           //used to stay in a state
	bool isNewState;              //true when the state has switched

	while(1){
	    distance = distanceInCm();
	    Clock_Delay1ms(20);
	}


//	while(1) {
//
//	    isNewState = (state != prevState);
//      prevState = state;
//	    istance = distanceInCm();  //this needs to be moved to the states that use it
//
//	    switch (state) {
//
//	        case FORWARD:
//	            Distance = distanceInCm();
//	            if(isNewState){
//	                MotorForward(14999, 14999);
//	            }
//	            MotorForward(14999, 14999);
//	            if(Distance <= 14){
//	                state = BACKWARD;
//	                }
//
//	            break;
//
//	        case BACKWARD:
//	            if(isNewState){
//	                //MotorBackward(14999, 14999);
//	                stateTimer = 0;
//	            }
//	            MotorBackward(14999, 14999);
//	            if(stateTimer >= 5){
//	                state = SWEEP_RIGHT;
//	            }
//	            break;
//
//            case RIGHT:
//                if(isNewState){
//                  stateTimer = 0;
//                }
//                MotorRight(7999, 7999);
//
//                if(stateTimer >= 5){
//                    state = FORWARD;
//                            }
//                break;
//
//            case LEFT:
//                if(isNewState){
//                  stateTimer = 0;
//                }
//                  MotorLeft(7999,7999);
//
//                if(stateTimer >= 5){
//                    state = FORWARD;
//                            }
//                break;
//
//            case SWEEP_RIGHT:
//                if(isNewState){
//                  stateTimer = 0;
//                }
//                Servo(); // move servo to right side
// 	              Clock_Delay1us(500);
//                RightDistance = distanceInCm();
//                if(stateTimer >= 5){
//                    state = SWEEP_LEFT;
//                }
//                break;
//
//            case SWEEP_LEFT:
//                if(isNewState){
//                  stateTimer = 0;
//                }
//                Servo();// move servo to left side
//	              Clock_Delay1us(500);
//	              LeftDistance = distanceInCm()
//                if(LeftDistance > 14 && RightDistance < 14 ){
//                    state = LEFT;
//                }
//                if(LeftDistance < 14 && RightDistance > 14 ){
//                    state = RIGHT;
//                }
//                break;
//
//                default: FORWARD;
//                } // switch case
//	      stateTimer++;
//        Clock_Delay1ms(20);
//	} //while
}
