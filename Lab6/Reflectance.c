// Reflectance.c
// Provide functions to take measurements using the kit's built-in
// QTRX reflectance sensor array.  Pololu part number 3672. This works by outputting to the
// sensor, waiting, then reading the digital value of each of the
// eight phototransistors.  The more reflective the target surface is,
// the faster the voltage decays.
// Daniel and Jonathan Valvano
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

// reflectance even LED illuminate connected to P5.3
// reflectance odd LED illuminate connected to P9.2
// reflectance sensor 1 connected to P7.0 (robot's right, robot off road to left)
// reflectance sensor 2 connected to P7.1
// reflectance sensor 3 connected to P7.2
// reflectance sensor 4 connected to P7.3 center
// reflectance sensor 5 connected to P7.4 center
// reflectance sensor 6 connected to P7.5
// reflectance sensor 7 connected to P7.6
// reflectance sensor 8 connected to P7.7 (robot's left, robot off road to right)

#include <stdint.h>
#include "msp432.h"
#include "..\inc\Clock.h"

// ------------Reflectance_Init------------
// Initialize the GPIO pins associated with the QTR-8RC
// reflectance sensor.  Infrared illumination LEDs are
// initially off.
// Input: none
// Output: none
void Reflectance_Init(void){
    // write this as part of Lab 6
    //    P5OUT &= ^0x08;
    //    P9OUT &= ^0x04;
    //P7OUT &= 0x00;
    P5DIR |= BIT3;
    P5OUT &= ~BIT3;
    P9DIR |= BIT2;
    P7DIR &= 0x00;
  }

// ------------Reflectance_Read------------
// Read the eight sensors
// Turn on the 8 IR LEDs
// Pulse the 8 sensors high for 100 ms
// Make the sensor pins input
// wait t us
// Read sensors
// Turn off the 8 IR LEDs
// Input: time to wait in usec
// Output: sensor readings
// Assumes: Reflectance_Init() has been called
uint8_t Reflectance_Read(uint32_t time){
      uint8_t result = 0x00;
        // write this as part of Lab 6
    //Turninf leds on - 2LEDS 5.3 and 9.2
      P5OUT |= BIT3; //  0000 0100
      P9OUT |= BIT2; // 0000 0100
      //Start charging sensors
      P7DIR |= 0xff;
      P7OUT |= 0xff;
      // Delay for 100ms to charge sensors
      Clock_Delay1ms(100);
      //Stop charging sensors by setting pins as input
      P7DIR &= 0x00;
      //delay 100ms and read inputs
      Clock_Delay1us(time);
      //turn off leds return port 7 read
      result = P7IN; // replace this line
      P5OUT &= ~BIT3; //  0000 0100
      P9OUT &= ~BIT2; // 0000 0100
      return result;
}

int32_t Reflectance_Position(uint8_t data){
   int32_t values[8] = {-334, -238, -142, -48, 48, 142, 238, 334}; // weighted values stored in array
   int32_t position = 0;
   int32_t zeroes = 0;
   int32_t weight = 0;
   int32_t dataSet = 0;
// new iteration -
   int i;
   for(i = 0; i < 8; i++){
       data >> i; // shift data right, will add zero to the Msb
       if(data & 0x01){
           zeroes++;
           weight += values[i];
       }
   }
   position = weight / zeroes;
   dataSet = position;
   weight = 0; // resets
   zeroes = 0; // resets
   position = 0; // resets
   return dataSet; // replace this line
}






// should go in main
// [7.7, 7.6, 7.5, 7.4, 7.3, 7.2, 7.1, 7.0]
// Perform sensor integration
// Input: data is 8-bit result from line sensor
// Output: position in 0.1mm relative to center of line
//int32_t Reflectance_Position(uint8_t data){
//    // write this as part of Lab 6
//    int32_t values[8] = {-334, -238, -142, -48, 48, 142, 238, 334,};
//    int32_t position = 0;
//    int32_t zeroes = 0;
//    int32_t weight = 0;
//    //To perform sensor integration you must use the sensor input, multiply
//    //the bit by its respective weight and add each of those values together,
//    //then divide by the total number of non-zeros bits.
//    int& element = vecOfNums[3];
//    if((data & 0x01)){
//        zeroes++;
//        int& weightBit0 = values[7];
//        weight += weightBit0;
//    }
//    if((data & 0x02)){
//        zeroes++;
//        int& weightBit1 = values[6];
//        weight += weightBit1;
//    }
//    if((data & 0x04) == 1){
//        zeroes++;
//        int& weightBit2 = values[5];
//        weight += weightBit2;
//    }
//    if((data & 0x08) == 1){
//        zeroes++;
//        int& weightBit3 = values[4];
//        weight += weightBit3;
//    }
//    if((data & 0x10) == 1){
//        zeroes++;
//        int& weightBit4 = vecOfNums[3];
//        weight += weightBit4;
//    }
//    if((data & 0x20) == 1){
//        zeroes++;
//        int& weightBit5 = vecOfNums[2];
//        weight += weightBit5;
//    }
//    if((data & 0x40) == 1){
//        zeroes++;
//        int& weightBit6 = vecOfNums[1];
//        weight += weightBit6;
//    }
//    if((data & 0x80) == 1){
//        zeroes++;
//        int& weightBit7 = vecOfNums[0];
//        weight += weightBit7;
//    }
//   position = weight / zeroes;
//   dataSet = position;
//   weight = 0;
//   zeroes = 0;
//   position = 0;
//   return dataSet; // replace this line
// }













