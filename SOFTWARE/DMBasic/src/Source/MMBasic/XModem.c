/***********************************************************************************************************************
MMBasic

xmodem.c

Handles all the custom commands and functions in MMBasic.  These are commands and functions that are not
normally part of the core BASIC language.  This is a good place to insert your own customised commands.

Copyright 2011 Geoff Graham - http://geoffg.net
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.  You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

************************************************************************************************************************/

#include <stdio.h>

#define INCLUDE_FUNCTION_DEFINES
#include "Maximite.h"
#include "MMBasic.h"
#include "Operators.h"
#include "Commands.h"
#include "External.h"
#include "Misc.h"
#include "Files.h"
#include "../Timers/Timers.h"

char *xmodemTransmit(int fnbr);
char *xmodemReceive(int fnbr);



/********************************************************************************************************************************************
 custom commands and functions
 each function is responsible for decoding a command
 all function names are in the form cmd_xxxx() (for a basic command) or fun_xxxx() (for a basic function) so, if you want to search for the
 function responsible for the NAME command look for cmd_name

 There are 4 items of information that are setup before the command is run.
 All these are globals.

 int cmdtoken	This is the token number of the command (some commands can handle multiple
				statement types and this helps them differentiate)

 char *cmdline	This is the command line terminated with a zero char and trimmed of leading
				spaces.  It may exist anywhere in memory (or even ROM).

 char *nextstmt	This is a pointer to the next statement to be executed.  The only thing a
				command can do with it is save it or change it to some other location.

 int CurrentLineNbr  This is read only and is set to zero if the command is in immediate mode.

 The only actions a command can do to change the program flow is to change nextstmt or
 execute longjmp(mark, 1) if it wants to abort the program.

 ********************************************************************************************************************************************/




void cmd_xmodem(void) {
	int rcv, fnbr;
	char *fname, *errorreturn;

	getargs(&cmdline, 3, " ");
	if(argc != 3) error("Invalid syntax");

	if(toupper(*argv[0]) == 'R')
		rcv = true;
	else if(toupper(*argv[0]) == 'S')
		rcv = false;
	else
		error("Invalid syntax");
		
	fname = getCstring(argv[2]);
	fnbr = FindFreeFileNbr();
	if(!fnbr) error("Too many files open");
	
	MMfopen(fname, rcv?"w":"r", fnbr);
	
	FileXfr = true;
	if(rcv)
		errorreturn = xmodemReceive(fnbr);
	else	
		errorreturn = xmodemTransmit(fnbr);
	FileXfr = false;

	MMfclose(fnbr);
	if(*errorreturn) error(errorreturn);
}


int _inbyte(int timeout) {
	int c;
	
	PauseTimer = 0;
	while(PauseTimer < timeout && !MMAbort) {
		c = MMInkey();
		if(c != -1) {
			return c;
		}	
	}
	return -1;	
}	

	
/***********************************************************************************************
the xmodem protocol
************************************************************************************************/

/* derived from the work of Georges Menie (www.menie.org) Copyright 2001-2010 Georges Menie
 * very much debugged and changed
 *
 * this is just the basic XModem protocol (no 1K blocks, crc, etc).  It has been tested on
 * Terra Term and is intended for use with that software.
 */


#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18

#define DLY_1S 1000
#define MAXRETRANS 25

#define X_BLOCK_SIZE	128
#define X_BUF_SIZE	X_BLOCK_SIZE + 6								// 128 for XModem + 3 head chars + 2 crc + nul


static int check(const unsigned char *buf, int sz)
{
	int i;
	unsigned char cks = 0;
	for (i = 0; i < sz; ++i) {
		cks += buf[i];
	}
	if (cks == buf[sz])
		return 1;

	return 0;
}


static void flushinput(void)
{
	while (_inbyte(((DLY_1S)*3)>>1) >= 0);
}


char *xmodemReceive(int fnbr) {
	unsigned char xbuff[X_BUF_SIZE];
	unsigned char *p;
	unsigned char trychar = NAK; //'C';
	unsigned char packetno = 1;
	int i, c;
	int retry, retrans = MAXRETRANS;

	// first establish communication with the remote
	while(1) {
		for( retry = 0; retry < 16; ++retry) {
			if(trychar) MMputchar(trychar);
			if ((c = _inbyte((DLY_1S)<<1)) >= 0) {
				switch (c) {
				case SOH:
					goto start_recv;
				case EOT:
					flushinput();
					MMputchar(ACK);
					return ""; 										// no more data
				case CAN:
					flushinput();
					MMputchar(ACK);
					return "Cancelled by remote"; 
					break;
				default:
					break;
				}
			}
		}
		flushinput();
		MMputchar(CAN);
		MMputchar(CAN);
		MMputchar(CAN);
		return "Remote did not respond";							// no sync

	start_recv:
		trychar = 0;
		p = xbuff;
		*p++ = SOH;
		for (i = 0;  i < (X_BLOCK_SIZE+3); ++i) {
			if ((c = _inbyte(DLY_1S)) < 0) goto reject;
			*p++ = c;
		}

		if (xbuff[1] == (unsigned char)(~xbuff[2]) && (xbuff[1] == packetno || xbuff[1] == (unsigned char)packetno-1) && check(&xbuff[3], X_BLOCK_SIZE)) {
			if (xbuff[1] == packetno)	{
				for(i = 0 ; i < X_BLOCK_SIZE ; i++)
					MMfputc(xbuff[i + 3], fnbr);
				++packetno;
				retrans = MAXRETRANS+1;
			}
			if (--retrans <= 0) {
				flushinput();
				MMputchar(CAN);
				MMputchar(CAN);
				MMputchar(CAN);
				return "Too many errors in transmission";
			}
			MMputchar(ACK);
			continue;
		}
	reject:
		flushinput();
		MMputchar(NAK);
	}
}


char *xmodemTransmit(int fnbr)
{
	unsigned char xbuff[X_BUF_SIZE]; 
	unsigned char packetno = 1;
	int i, c, len = 0;
	int retry;

	// first establish communication with the remote
	while(1) {
		for( retry = 0; retry < 16; ++retry) {
			if ((c = _inbyte((DLY_1S)<<1)) >= 0) {
				switch (c) {
				case NAK:											// start sending
					goto start_trans;
				case CAN:
					if ((c = _inbyte(DLY_1S)) == CAN) {
						MMputchar(ACK);
						flushinput();
						return "Cancelled by remote"; 
					}
					break;
				default:
					break;
				}
			}
		}
		MMputchar(CAN);
		MMputchar(CAN);
		MMputchar(CAN);
		flushinput();
		return "Remote did not respond";							// no sync

		// send a packet
		while(1) {
		start_trans:
			memset (xbuff, 0, X_BUF_SIZE);							// start with an empty buffer
			
			xbuff[0] = SOH;											// copy the header
			xbuff[1] = packetno;
			xbuff[2] = ~packetno;
			
			for(len = 0; len < 128 && !MMfeof(fnbr); len++) {
				xbuff[len + 3] = MMfgetc(fnbr);						// copy the data into the packet
			}

			if (len > 0) {
				unsigned char ccks = 0;
				for (i = 3; i < X_BLOCK_SIZE+3; ++i) {
					ccks += xbuff[i];
				}
				xbuff[X_BLOCK_SIZE+3] = ccks;
				
				// now send the block
				for (retry = 0; retry < MAXRETRANS && !MMAbort; ++retry) {
					// send the block
					for (i = 0; i < X_BLOCK_SIZE+4 && !MMAbort; ++i) {
						MMputchar(xbuff[i]);
					}
					// check the response
					if ((c = _inbyte(DLY_1S)) >= 0 ) {
						switch (c) {
						case ACK:
							++packetno;
							goto start_trans;
						case CAN:									// canceled by remote
							MMputchar(ACK);
							flushinput();
							return "Cancelled by remote";  
							break;
						case NAK:									// receiver got a corrupt block
						default:
							break;
						}
					}
				}
				// too many retrys... give up
				MMputchar(CAN);
				MMputchar(CAN);
				MMputchar(CAN);
				flushinput();
				return "Too many errors in transmission";
			}
			
			// finished sending - send end of text
			else {
				for (retry = 0; retry < 10; ++retry) {
					MMputchar(EOT);
					if ((c = _inbyte((DLY_1S)<<1)) == ACK) break;
				}
				flushinput();
				if(c == ACK) return "";
				return "Error closing communications";
			}
		}
	}
}

