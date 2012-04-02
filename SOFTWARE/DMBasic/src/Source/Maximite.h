/***********************************************************************************************************************
MMBasic

Maximite.h

Top level header file for the Maximite Computer

Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

************************************************************************************************************************/


#ifndef MMHeader
#define MMHeader

	#define VERSION         "2.7B"					// define the version number
	#define YEAR			__DATE__				// current date
	
	// The main clock frequency for the chip
	#define	CLOCKFREQ		(80000000L)				// This is set in in Configuration Bits.h

	// The peripheral bus frequency
	#define BUSFREQ			(CLOCKFREQ/1)			// This is set in the first few lines of main.c
	
	// standard type definitions
	typedef     char            bit;
	typedef     char     		sint8;
	typedef     short int    	sint16;
	typedef     int    			sint24;
	typedef     int    			sint32;
	typedef     unsigned char  	uint8;
	typedef     unsigned short int  uint16;
	typedef     unsigned int  	uint24;
	typedef     unsigned int  	uint32;

	typedef 	unsigned char	byte;

	#define nop	__asm__ ("NOP")

	#define forever 1
	#define true	1
	#define false	0
	//#define OFF     0
	//#define ON      1

	#if defined(__DEBUG)
		#define dp(...) {char s[40];sprintf(s,  __VA_ARGS__); MMPrintString(s); MMPrintString("\r\n");}
	    #define debughalt() __asm__ volatile (" sdbbp 0")
		void dump(char *p, int nbr);
	#else
	    #define debughalt() (void)0
		#define dp(...) {;}
	#endif

	// for both of these macros the maximum delay is 4 seconds
	#define uSec(a)		{WriteCoreTimer(0); while(ReadCoreTimer() < (((a * 1000) - 320) / (2000000000/CLOCKFREQ)));}
	#define mSec(a)		{WriteCoreTimer(0); while(ReadCoreTimer() < (((a * 1000000) - 320) / (2000000000/CLOCKFREQ)));}
/*
	#ifdef __DEBUG
		// Debugging console. Functions available include DBPRINTF(),DBSCANF(),DBGETC(),DBGETWORD(),DBPUTC(),DBPUTWORD()
		// note speed is reduced by 1/3 in debug mode (ie 20MHz in debug vs 60MHz in release mode)
	  	#define _APPIO_DEBUG											// comment out to disable console debugger
	  	#include <sys/appio.h>
	#endif
*/
	#define SDActivityTime 40

	// functions for processing USB data (defined in Main.c)
	#define INP_QUEUE_SIZE	256
	extern volatile unsigned char InpQueue[INP_QUEUE_SIZE];
	extern volatile int InpQueueHead, InpQueueTail;

	// functions for getting data from keyboard/USB (defined in Main.c)
	extern int MMInkey(void);
	extern char MMGetc(void);
	extern void AddToKeystrokeBuffer(unsigned char c);

	// functions for sending data to keyboard/USB (defined in Main.c)
	extern void MMPrintString(char *);
	extern char PrintSignonToUSB;
	extern void VideoPutc(char);
	extern char MMPutc(char c);
	extern int MMgetchar(void);
	extern char MMputchar(char c);
	extern void USBPutchar(char c);
	
	// MMBasic variables defined in Main.c
	extern int MMCharPos;
	extern volatile int MMAbort;
	extern int FileXfr;
	extern int Autorun;
	extern int USBOn, VideoOn;										// variables controlling the display of the output
        extern volatile int DisableMMAbort;
        extern char MSDEnable;
	// Uncomment the following #define to turn on performance profiling
	// To use profiling run a program.  When it terminates the results will be written to the SD card as PROFILE.XLS
	// Note:  The serial ports cannot be used while profiling and execution speed will be reduced by approx 10%
	//#define PROFILE
	
	#ifdef PROFILE
		#define P_START_ADDR	0x9d004800							// start address to profile
		#define P_END_ADDR		0x9d070000							// this must be increased if more code is added
		#define P_GRANUALITY	64									// reporting increments
		void StartProfiling(void);
		void StopProfiling(void);
	#endif

#endif
