/***********************************************************************************************************************
MMBasic

serial.h

Include file that contains the globals and defines for serial.c in MMBasic.
  
Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>. 

************************************************************************************************************************/

#define	COM_MAX_BAUD_RATE			19200
#define	COM_DEFAULT_BUF_SIZE                    128
#define COM_FLOWCTRL_MARGIN			6

// global variables
extern int SerialConsole;											// holds the com number for the console function or zero if disabled
extern char *com1_interrupt;										// pointer to the interrupt routine
extern int com1_ilevel;												// number nbr of chars in the buffer for an interrupt
extern char *com2_interrupt;										// pointer to the interrupt routine
extern int com2_ilevel;												// number nbr of chars in the buffer for an interrupt

#ifdef OLIMEX
extern char *com3_interrupt; // pointer to the interrupt routine
extern int com3_ilevel; // number nbr of chars in the buffer for an interrupt
extern char *com4_interrupt; // pointer to the interrupt routine
extern int com4_ilevel; // number nbr of chars in the buffer for an interrupt
#endif


extern int com1_buf_size;											// buffer size
extern int com2_buf_size;

#ifdef OLIMEX
extern int com3_buf_size; // buffer size
extern int com4_buf_size;
#endif

// global functions
void SerialOpen(char *spec, int as_console);
void SerialClose(int comnbr);
unsigned char SerialPutchar(int comnbr, unsigned char c);
int SerialRxStatus(int comnbr);
int SerialTxStatus(int comnbr);
int SerialGetchar(int comnbr);
