/***********************************************************************************************************************
Maximite

keyboard.c

Does all the hard work in getting data from the PS2 keyboard

This routine is based on a technique and code presented by Lucio Di Jasio in his excellent book 
"Programming 32-bit Microcontrollers in C - Exploring the PIC32".
  
Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>. 

************************************************************************************************************************/

#define INCLUDE_FUNCTION_DEFINES

#include <p32xxxx.h>
#include <plib.h>
#include "Keyboard.h"
#include "../Maximite.h"
#include "../IOPorts.h"
#include "MMBasic/external.h"
#include "../Setup.h"
#include "../DuinoMite/RTC.h"

// definition of the keyboard PS/2 state machine 
#define PS2START    0
#define PS2BIT      1
#define PS2PARITY   2
#define PS2STOP     3

// PS2 KBD state machine and buffer
int PS2State;
unsigned char KBDBuf;
int KState, KCount, KParity;

// key codes that must be tracked for up/down state
#define CTRL  		0x14			// left and right generate the same code
#define L_SHFT  	0x12 
#define R_SHFT  	0x59
#define CAPS    	0x58


// this is a map of the keycode characters and the character to be returned for the keycode
const char keyCodes[128]={    
                0,      F9,       0,      F5,      F3,      F1,      F2,     F12,       //00
                0,     F10,      F8,      F6,      F4,     TAB,     '`',       0,       //08
                0,     ALT,  L_SHFT,      0,     CTRL,     'q',     '1',       0,       //10
                0,       0,     'z',     's',     'a',     'w',     '2',       0,       //18
                0,     'c',     'x',     'd',     'e',     '4',     '3',       0,       //20
                0,     ' ',     'v',     'f',     't',     'r',     '5',       0,       //28
                0,     'n',     'b',     'h',     'g',     'y',     '6',       0,       //30
                0,       0,     'm',     'j',     'u',     '7',     '8',       0,       //38
                0,     ',',     'k',     'i',     'o',     '0',     '9',       0,       //40
                0,     '.',     '/',     'l',     ';',     'p',     '-',       0,       //48
                0,       0,    '\'',       0,     '[',     '=',       0,       0,       //50
             CAPS,  R_SHFT,   ENTER,     ']',      0,     0x5c,       0,       0,       //58
                0,       0,       0,       0,       0,       0,    BKSP,       0,       //60
                0,     '1',       0,     '4',     '7',       0,       0,       0,       //68
              '0',     '.',     '2',     '5',     '6',     '8',     ESC,       0,       //70
              F11,     '+',     '3',     '-',     '*',     '9',       0,       0        //78
            };

// this map is with the shift key depressed
const char keySCodes[128] = {
                0,      F9,       0,      F5,      F3,      F1,      F2,     F12,       //00
                0,     F10,      F8,      F6,      F4,     TAB,     '~',       0,       //08
                0,     ALT,  L_SHFT,      0,     CTRL,     'Q',     '!',       0,       //10
                0,       0,     'Z',     'S',     'A',     'W',     '@',       0,       //18
                0,     'C',     'X',     'D',     'E',     '$',     '#',       0,       //20
                0,     ' ',     'V',     'F',     'T',     'R',     '%',       0,       //28
                0,     'N',     'B',     'H',     'G',     'Y',     '^',       0,       //30
                0,       0,     'M',     'J',     'U',     '&',     '*',       0,       //38
                0,     '<',     'K',     'I',     'O',     ')',     '(',       0,       //40
                0,     '>',     '?',     'L',     ':',     'P',     '_',       0,       //48
                0,       0,    '\"',       0,     '{',     '+',       0,       0,       //50
             CAPS,  R_SHFT,   ENTER,     '}',       0,     '|',       0,       0,       //58
                0,       0,       0,       0,       0,       0,    BKSP,       0,       //60
                0,     '1',       0,     '4',     '7',       0,       0,       0,       //68
               '0',    '.',     '2',     '5',     '6',     '8',     ESC,       0,       //70
              F11,     '+',     '3',     '-',     '*',     '9',       0,       0        //78
            };

// this map is for when the keycode preceeded by 0xe0
const char keyE0Codes[7*8] = {
				0,       0,     '/',       0,       0,       0,       0,       0,    	//48
				0,       0,       0,       0,       0,       0,       0,       0,    	//50
				0,       0, NUM_ENT,       0,       0,       0,       0,       0,    	//58
				0,       0,       0,       0,       0,       0,       0,       0,    	//60
				0,     END,       0,    LEFT,    HOME,       0,       0,       0,    	//68
		   INSERT,     DEL,    DOWN,       0,   RIGHT,      UP,       0,    NUML,    	//70
				0,       0,   PDOWN,       0,       0,     PUP,   SLOCK,       0,    	//78
            };



/***************************************************************************************************
InitKeyboard
Initialise the keyboard and sound routines.
****************************************************************************************************/
void initKeyboard(void) {

	// enable pullups on the clock and data lines.  
	// This stops them from floating and generating random chars when no keyboard is attached
 	CNCONbits.ON = 1;       						// turn on Change Notification module
 	P_PS2CLK_PULLUP = P_ON;							// turn on the pullup for pin D6 also called CN15
 	P_PS2DAT_PULLUP = P_ON;							// turn on the pullup for pin D7 also called CN16

    // setup Change Notification interrupt 
    P_PS2CLK_INT = P_ON;    							// enable PS2CLK (CN15) as a change interrupt
    mCNSetIntPriority(3);  							// set interrupt priority to 3
    mCNClearIntFlag();      							// clear the interrupt flag
    mCNIntEnable(1);       							// enable interrupt
}



// change notification interrupt service routine
void __ISR( _CHANGE_NOTICE_VECTOR, ipl3) CNInterrupt(void) {
    int d;
	unsigned char c;
	
	static char LShift = 0;
	static char RShift = 0;
	static char Ctrl = 0;
	static char CapsLock = 0;
	static char KeyUpCode = false;
	static char KeyE0 = false;
	static unsigned char Code = 0;

    // Make sure it was a falling edge
    if(P_PS2CLK == 0)
    {
	
	    // Sample the data
	    d = P_PS2DAT;
	
        switch(PS2State){
            default:
            case PS2START:   
                if(!d) {                							// PS2DAT == 0
                    KCount = 8;         							// init bit counter
                    KParity = 0;        							// init parity check
                    Code = 0;
                    PS2State = PS2BIT;
                }
                break;

            case PS2BIT:      
                Code >>= 1;            								// shift in data bit
                if(d) Code |= 0x80;                					// PS2DAT == 1
                KParity ^= Code;      								// calculate parity
                if (--KCount == 0) PS2State = PS2PARITY;   			// all bit read
                break;

            case PS2PARITY:         
                if(d) KParity ^= 0x80;                 				// PS2DAT == 1
                if (KParity & 0x80)    								// parity odd, continue
                    PS2State = PS2STOP;
                else
                    PS2State = PS2START;   
                break;

            case PS2STOP:    
                if(d) {                 							// PS2DAT == 1
	                if(Code == 0xf0)
	                	KeyUpCode = true;
	                else if(Code == 0xe0)
	                	KeyE0 = true;
	                else {
		                // Process a scan code from the keyboard into an ASCII character.  It then inserts it into the keyboard queue.
						if(KeyUpCode) {
						    if(Code == L_SHFT) LShift = 0;								// left shift button is released
						    if(Code == R_SHFT) RShift = 0;								// right shift button is released
						    if(Code == CTRL) Ctrl = 0;									// left control button is released
						    goto SkipOut;
						}
						
						// we are only here if the key has been pressed (NOT released)
						
					    if(Code == L_SHFT) { LShift = 1; goto SkipOut; }				// left shift button is pressed
					    if(Code == R_SHFT) { RShift = 1; goto SkipOut; }				// right shift button is pressed
					    if(Code == CTRL) { Ctrl = 1; goto SkipOut; }					// left control button is pressed
						if(Code == CAPS) { CapsLock = !CapsLock; goto SkipOut; }		// caps lock pressed
					
						// now get the character into c - or why, oh why, are scan codes so random?
						if(KeyE0 && Code == 0x11)
							c = ALT;													// a special case
						else if(!KeyE0 && Code == 0x83) 
							c = F7;														// a second special case
						else if(KeyE0) {
							if(Code >= 0x48 && Code < 0x80)
								c = keyE0Codes[Code - 0x48];							// a keycode preceeded by 0xe0
							else
								c = 0;													// invalid char
						} else {
					    	if(LShift || RShift)
					        	c = keySCodes[Code%128];								// a keycode preceeded by a shift
					    	else
					        	c = keyCodes[Code%128];									// just a keycode
					 	}       	
						
						if(!c) goto SkipOut;
						
						if(c <= 0x7f) {													// a normal character
							if(CapsLock && c >= 'a' && c <= 'z') c -= 32;				// adj for caps lock
							if(Ctrl) c &= 0x1F;								// adj for control
						} else	{														// must be a function key or similar
							if(LShift || RShift) c |= 0b00100000;
							if(Ctrl) c |= 0b01000000;
						}
						
						if(!FileXfr) {													// don't clutter the queue if a file transfer is underway
                                                    if(S.ScreenSave && !ScreenSaveTime)     //if screen saver is enabled we do this here so the keypress does not go thru
                                                    {
                                                        ScreenSaveTime=(S.ScreenSave*60);   //check if its zero then reset counter turn video on
                                                        mT3IntEnable(1);        // turn on video int
                                                    }
                                                    else
                                                    {
                                                        ScreenSaveTime=(S.ScreenSave*60);
                                                        InpQueue[InpQueueHead] = c;									// place into the queue
							InpQueueHead = (InpQueueHead + 1) % INP_QUEUE_SIZE;			// increment the head index
                                                    }
                                                }
						
						if(c == 3  && !DisableMMAbort) {													// check for CTRL-C
							MMAbort = true;
							InpQueueHead = InpQueueTail = 0;							// flush the input buffer
						}	
						PrintSignonToUSB = false;										// show that the user is using the keyboard
						
						SkipOut:
						// end lump of self contained code
						//////////////////////////////////////////////////////////////////////////////////////////////////////////
	                	KeyUpCode = false;
	                	KeyE0 = false;
	                }	
	            Code = 0;
                }    
                PS2State = PS2START;
                break;
	    } 
	}
        
        // clear interrupt flag
        SleepMMVal=SleepCNInt;
        mCNClearIntFlag();
}	
