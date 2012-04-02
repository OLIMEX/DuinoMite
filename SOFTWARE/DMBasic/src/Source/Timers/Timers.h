/***********************************************************************************************************************
MMBasic

timers.h

Include file that contains the globals and defines for timers.c in MMBasic.
  
Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>. 

************************************************************************************************************************/

// timer variables
extern volatile unsigned int mSecTimer;										// this is used to count mSec
extern volatile unsigned int PauseTimer;										// this is used in the PAUSE command
extern volatile unsigned int IntPauseTimer;									// this is used in the PAUSE command
extern volatile unsigned int TickTimer;										// used in the tick interrupt
extern volatile unsigned int CursorTimer;									// used to control cursor blink rate
extern volatile unsigned int InkeyTimer;										// used to delay on an escape character

// date/time counters
extern volatile int second;													
extern volatile int minute;
extern volatile int hour;
extern volatile int day;
extern volatile int month;
extern volatile int year;

#ifdef OLIMEX
extern volatile int dow;
#endif

// sound variables
extern volatile int SoundPlay;

// SD Card Activity
extern volatile int SDActivityLED;
extern volatile int SDCardRemoved;

// global timer functions
extern void initTimers(void);

// define the blink rate for the cursor
#define CURSOR_OFF		350											// cursor off time in mS
#define CURSOR_ON		650											// cursor on time in mS

