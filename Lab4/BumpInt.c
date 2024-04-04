// BumpInt.c
// Runs on MSP432, interrupt version
// Provide low-level functions that interface bump switches on the robot.
// Daniel Valvano and Jonathan Valvano
// July 11, 2019

/* This example accompanies the book
   "Embedded Systems: Introduction to Robotics,
   Jonathan W. Valvano, ISBN: 9781074544300, copyright (c) 2019
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2019, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/

// Negative logic bump sensors
// P4.7 Bump5, left side of robot
// P4.6 Bump4
// P4.5 Bump3
// P4.3 Bump2
// P4.2 Bump1
// P4.0 Bump0, right side of robot
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
#include "..\inc\Motor.h"


#include <stdint.h>
#include "msp.h"
// Initialize Bump sensors
// Make six Port 4 pins inputs
// Activate interface pullup
// pins 7,6,5,3,2,0
// Interrupt on falling edge (on touch)
#define B0 0x02 // 0b0000 0000 P4.0
#define B1 0x06 // 0b0000 0100 p4.2
#define B2 0x08 // 0b0000 1000 p4.3
#define B3 0x0C // 0b0010 0000 p4.5
#define B4 0x0E // 0b0100 0000 p4.6
#define B5 0x10 // 0b1000 0000 p4.7

volatile bool wasInterrupt = 0;
volatile bool is_Left = 0;
volatile bool is_Right = 0;

void BumpInt_Init(void){
    P4DIR &=  0b00010010; // pins are inputs
    P4REN |=  0b11101101; // enable resistors
    P4OUT |=  0b11101101; // pull ups
    P4IE  |=  0b11101101; // enable interrupt
    P4IES |=  0b11101101; // trigger falling edge
    P4IFG = 0x00; // clear flag
    NVIC -> ISER[1] |= 0x000000000040;
}

// triggered on touch, falling edge
void PORT4_IRQHandler(void){
    // write this as part of Lab 4
       static int16_t count = 0;
       uint16_t counts = P4IV;
       wasInterrupt = 1;

       if (counts == B0) {
           count += 3;
          // is_Left = 1;
       }

       else if  (counts == B1) {
              count += 2;
           //   is_Left = 1;
          }

       else if (counts == B2) {
              count += 1;
             // is_Left = 1;
      }

       else if (counts == B3) {
              count -= 1;
             // is_Right = 1;
          }

       else if (counts == B4) {
              count -= 2;
            //  is_Right = 1;
          }

       else if (counts == B5 ) {
              count -= 3;
            //  is_Right = 1;
          }
       if(count >= 0){
           count =0;
           is_Right = 1;
           is_Left = 0;
       }else{
           count =0;
           is_Left = 1;
           is_Right = 0;
       }

//
//           if(count < 0){
//           is_Left = 1;
//       }
       P4IFG &= 0x00; // clear interrupt flags
}

