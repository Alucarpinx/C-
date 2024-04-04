#include <msp.h> 
#include <stdint.h>
#include <stdbool.h>

#define Start BIT7 //Port2                              
#define Full BIT5   //Port2
#define Empty BIT4 //Port2
#define Fill_Valve BIT6 //Port5
#define Empty_Valve BIT1 //Port5
#define Drum_Motor BIT2  //Port5


void main() {

   WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer - always include this   
    
    
//These are the seven states of the state machine
enum washing_machine {IDLE, FILL, AGITATE, DRAIN, RINSE, SPIN} state, prevState;

state = IDLE;          //start in OFF state
prevState = !IDLE;   //used to know when the state has changed
uint8_t stateTimer;           //used to stay in a state
volatile bool isNewState;              //true when the state has switched
volatile bool START;              
volatile bool FULL;             
volatile bool EMPTY; 
volatile uint16_t i;        // iteration counter for delay loop


    P5DIR |= 0b01000110;     //make P1.7, P1.6, P1.5, P1.4, P1.3, P1.2 outputs
    P5OUT &= 0b01000110;     //Set all outputs to zero
    P2DIR &= 0b10110000;     //make P2.0 and P2.1 inputs

    while(1) {
        // read inputs
        START = ((P2IN & BIT7) == BIT7);
        FULL = ((P2IN & BIT5) == BIT5);
        EMPTY = ((P2IN & BIT4) == BIT4);
        
        //state information
        isNewState = (state != prevState); 
        prevState = state;  //save state for next time

        switch (state) {       
            case IDLE:
                //entry housekeeping
                P5OUT &= 0b01000110; //turn off all outputs. 
                //Fill_Valve = 0
                //Drum_Motor = 0
                //Empty_Valve = 0
             
                if (START) {          //exit condition
                    state = FILL;
                }
            break;
            case FILL:
                //entry housekeeping
                if (isNewState) {
                    stateTimer = 0;
                }
                
                P5OUT |= Fill_Valve; //Opens Fill_Valve
                //Fill_Valve = 1
                //Drum_Motor = 0
                //Empty_Valve = 0
                
                //state business
                stateTimer++;  //increment timer
                
                if (FULL) {       //exit condition
                    state = AGITATE;
                }
            break;
            case AGITATE:
                //entry housekeeping
                if (isNewState) {
                    stateTimer = 0;
                }
                
                P5OUT &= 0b10111111; //Closes Fill_Valve
                P5OUT |= Drum_Motor; //Turns on Drum_Motor                      
                //Fill_Valve = 0
                //Drum_Motor = 1
                //Empty_Valve = 0   <- already off
                                        
                stateTimer++;  //increment timer
                
                if (stateTimer == 7) {                 //exit condition 1
                    state = DRAIN;
                }
            break;
            case DRAIN:
                //entry housekeeping
                if (isNewState) {
                    stateTimer = 0;
                }
                
                P5OUT &= 0b11111011; //Turns Drum_Motor off                    
                P5OUT |= Empty_Valve;   //Opens Empty_Valve                     
                //Fill_Valve = 0    <- Already closed
                //Drum_Motor = 0
                //Empty_Valve = 1                     
                                     
                stateTimer++;  //increment timer
                 
                if (EMPTY){   //exit condition 
                    state = RINSE;
                }
            break;
            case RINSE:
                //entry housekeeping
                if (isNewState) {
                    stateTimer = 0;
                }
                
                P5OUT &= 0b11111101; //Closes Empty_Valve                 
                P5OUT |= Fill_Valve;   //Opens Fill_valve  
                //Fill_Valve = 1
                //Drum_Motor = 0  <- Already off
                //Empty_Valve = 0
                
                //state business
                stateTimer++;  //increment timer
                
                if (FULL) {   //exit condition 
                    state = SPIN;
                }
            break;
            case SPIN:
                //entry housekeeping
                if (isNewState) {
                    stateTimer = 0;
                }
                
                P5OUT &= 0b10111111; //Closes Fill_Valve                 
                P5OUT |= 0b00000110;   //Opens Empty_valve and Turns on Drum_Motor                       
                //Fill_Valve = 0   
                //Drum_Motor = 1
                //Empty_Valve = 1   
                                       
                stateTimer++;  //increment timer
                
                if ((stateTimer == 10) | (EMPTY)) {    //exit conditions
                    state = IDLE;
                }
            break;

            default: state = IDLE;
        } //end switch
    for(i=10000; i>0; i--);  //random delay loop
    } //end while
} //end main
