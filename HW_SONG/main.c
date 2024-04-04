
#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include "..\inc\Clock.h"
#include "..\inc\CortexM.h"
#include "..\inc\Init_Ports.h"

//Note Constants
#define LA 13664//220
#define LAS 12882//233
#define LB 12252//245
#define LC 11441 //262
#define LCS 10840 //277
#define LD 10210 //294
#define LDS 9639 //311
#define LE 9099 //330
#define LF 8618 //349
#define LFS 8108 //370
#define LG 7657 //392
#define LGS 7237 //415
#define MA 6816 //440
#define MAS 6456 //466
#define MB 6066 //494
#define MC //523
#define MCS 554
#define MD 587
#define MDS 622
#define ME 659
#define MF 698
#define MFS 740
#define MG 783
#define MGS 830
#define HA 880

//Time constants
#define wholeNote   3000000
#define dottedHalf  2250000
#define halfNote    1500000
#define quarterNote 750000

void playNote(int noteTime, int note);

void main(void){
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	//PORT_INIT
	P2DIR |= 0b00010001;
	//P2OUT &= 0b00010000;
	P2SEL1 &= ~0x10;
	P2SEL0 |= 0x10;
	TA0CTL = 0x0210;
	TA0CCTL1 |= 0x00E0;

	TA0CCR0 = 0;     // set period of wave
	TA0CCR1 = 0;  //set high time of wave

	while(1){
	    playNote(halfNote, LE);
	    playNote(halfNote, LD);
	    playNote(wholeNote, LC);
	    playNote(halfNote, LE);
	    playNote(halfNote, LD);
	    playNote(wholeNote, LC);
	    playNote(quarterNote, LC);
	    playNote(quarterNote, LC);
	    playNote(quarterNote, LC);
	    playNote(quarterNote, LC);
	    playNote(quarterNote, LD);
        playNote(quarterNote, LD);
        playNote(quarterNote, LD);
        playNote(quarterNote, LD);
	    playNote(halfNote, LE);
	    playNote(halfNote, LD);
	    playNote(wholeNote, LC);

	}
}


void playNote(int noteTime, int note){
    //TA0CTL  |= 0x0010;
    //TA0CCRN
    TA0CTL &= ~0x0030;//stop the timer
    TA0R = 0;//clear the timer
   //note = (1/note) /0.000000333; //T = 333ns
    TA0CCR0 = note;     // set period of wave
    TA0CCR1 = note /2;  //set high time of wave

    TA0CTL |= 0x0010; //mode up     start the wave

    SysTick -> LOAD = noteTime;   // load note period
    SysTick -> VAL = 0;           // reset current value
    SysTick -> CTRL = 0x00000005; //
    while (!(SysTick -> CTRL & 0x00010000));
    TA0CTL &= 0x0030;//stop the timer
    /// delay 50 ms
    SysTick -> LOAD = 150150;   // 50ms count
    SysTick -> VAL = 0;           // reset current value
    SysTick -> CTRL = 0x00000005; //
    while (!(SysTick -> CTRL & 0x00010000));


    P2OUT ^= 0x01;
    return;
}
