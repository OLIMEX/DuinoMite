/***********************************************************************************************************************
Maximite

timers.c

This module manages various timers (counting variables), the date/time,
counting inputs and generates the sound.  All this is contained within the timer 4 interrupt.
  
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
#include "Timers.h"
#include "../Maximite.h"
#include "../IOPorts.h"
#include "../Video/Video.h"
#include "MMBasic/external.h"
//	GS I2C Start
#include "MMBasic/I2C.h"
//	GS I2C End
#include "Setup.h"

// timer variables
volatile unsigned int SecondsTimer = 0;
volatile unsigned int PauseTimer = 0;
volatile unsigned int IntPauseTimer = 0;
volatile unsigned int TickTimer = 0;
volatile unsigned int CursorTimer = 0;
volatile unsigned int InkeyTimer = 0;

// sound variables
volatile int SoundPlay;

volatile unsigned int mSecTimer = 0;													// this is used to count mSec
volatile int second = 0;														// date/time counters
volatile int minute = 0;
volatile int hour = 0;
volatile int day = 1;
volatile int month = 1;
volatile int year = 2000;
#ifdef OLIMEX
volatile int dow =0;
#endif
volatile int SDActivityLED = 0;

const char DaysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

volatile int SDCardRemoved = true;
#ifdef OLIMEX
volatile char SDLedState=0;
#endif


/***************************************************************************************************
InitTimers
Initialise the 1 mSec timer used for internal timekeeping.
****************************************************************************************************/
void initTimers(void) {

 	// setup timer 4
    PR4 = 1 * ((BUSFREQ/2)/1000) - 1;       					// 1 mSec
    T4CON = 0x8010;         							// T4 on, prescaler 1:2
    mT4SetIntPriority(1);  							// lower priority
    mT4ClearIntFlag();      							// clear interrupt flag
    mT4IntEnable(1);       							// enable interrupt 
    P_SD_ACTIVITY_TRIS = 0; P_SD_LED_SET_LO;				// initialise the SD card activity led (maintained by timers.c)
}



/****************************************************************************************************************
Timer 4 interrupt processor
This fires every millisecond and is responsible for tracking the time and the counts of various timing variables
*****************************************************************************************************************/void __ISR( _TIMER_4_VECTOR, ipl1) T4Interrupt(void) {

	/////////////////////////// count up timers /////////////////////////////////////
	//if(ExtCurrentConfig[11] == EXT_PER_IN) INT1Count++;			// if we are measuring period increment the count
	if(ExtCurrentConfig[5] == EXT_PER_IN) INT2Count++;
	if(ExtCurrentConfig[6] == EXT_PER_IN) INT3Count++;
	if(ExtCurrentConfig[7] == EXT_PER_IN) INT4Count++;
	mSecTimer++;                                                            // used by the TIMER function
	TickTimer++;								// used in the interrupt tick
	PauseTimer++;								// used by the PAUSE command
	IntPauseTimer++;							// used by the PAUSE command inside an interrupt
	InkeyTimer++;								// used to delay on an escape character
	if(++CursorTimer > CURSOR_OFF + CURSOR_ON) CursorTimer = 0;		// used to control cursor blink rate
//	GS I2C Start
	if (I2C_Timer) {
		if (--I2C_Timer == 0) {
			I2C_Status |= I2C_Status_Timeout;
			mI2C1MSetIntFlag();
		}
	}
	if (I2C_Status & I2C_Status_MasterCmd) {
		if (!(I2C1STAT & _I2C1STAT_S_MASK)) {
			I2C_Status &= ~I2C_Status_MasterCmd;
			I2C_State = I2C_State_Start;
			I2C1CONSET =_I2C1CON_SEN_MASK;
		}
	}
//	GS I2C End
#ifdef MAXIMITE
	if(SDActivityLED) {
		P_SD_LED_SET_HI;
		SDActivityLED--;
	} else
		P_SD_LED_SET_LO;
#endif
	
	if(SD_CD) SDCardRemoved = true;

	// check if the sound has expired
	if(SoundPlay > 0) {												// if we are still playing the sound
		SoundPlay--;
		if(SoundPlay == 0) {
			CloseTimer2();
#ifdef MAXIMITE
        CloseOC2();
#endif
#ifdef OLIMEX
        CloseOC1();
#endif
		}
	}		


	//////////////////////////////// keep track of the date and time ////////////////////////////////
	////////////////////////////////// this code runs once a second /////////////////////////////////
	if(++SecondsTimer >= 1000) {
		SecondsTimer = 0;											// reset every second
                if(S.ScreenSave){           //if screen saver is enabled count it down
                if(ScreenSaveTime >0)
                    ScreenSaveTime--;
                else
                mT3IntEnable(0);        // turn off video int
                }
                //if(ExtCurrentConfig[11] == EXT_FREQ_IN) { INT1Value = INT1Count; INT1Count = 0; }
		if(ExtCurrentConfig[5] == EXT_FREQ_IN) { INT2Value = INT2Count; INT2Count = 0; }
		if(ExtCurrentConfig[6] == EXT_FREQ_IN) { INT3Value = INT3Count; INT3Count = 0; }
		if(ExtCurrentConfig[7] == EXT_FREQ_IN) { INT4Value = INT4Count; INT4Count = 0; }
#ifdef MAXMITE
                if(++second >= 60) {										// keep track of the time and date
			second = 0 ;
			if(++minute >= 60) {
				minute = 0;
				if(++hour >= 24) {
					hour = 0;
					if(++day > DaysInMonth[month + ((month == 2 && (year % 4) == 0)?1:0)]) {
						day = 1;
						if(++month > 12) {
							month = 1;
							year++;
						}
					}
				}
			}
		}
#endif
        }

    // Clear the interrupt flag
    mT4ClearIntFlag();
    //return;
}
